#include <mpi.h>
#include <set>
#include "bounds.hh"
#include "Sender.hh"
#include "errors.hh"
#include "version.hh"


template <typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& v) {
    std::size_t total_size = 0;
    for (const auto& sub : v)
        total_size += sub.size(); // I wish there was a transform_accumulate
    std::vector<T> result;
    result.reserve(total_size);
    for (const auto& sub : v)
        result.insert(result.end(), sub.begin(), sub.end());
    return result;
}


std::vector<Point> Sender::sent_initial_all(const std::vector<Point>& __data) {
    int m_err;
    std::vector<Point> result;
    if (this->process_number == 0) {
        m_err = MPI_Bcast((void *) &__data.front(), __data.size(), Point::mpi_type, 0, MPI_COMM_WORLD);
        result = __data;
    } else {
        result.resize(this->gal_1_stars + this->gal_2_stars);
        m_err = MPI_Bcast((void *) &result.front(), result.size(), Point::mpi_type, 0, MPI_COMM_WORLD);
    }
    if (m_err != 0) {
        handle_error("sending initial data");
    }

    return result;
}

constexpr static inline int positive_module(const int& a, const int& b) {
    return (a + b) % b;
}

std::vector<std::vector<Point> > Sender::distribute_neighbours(const std::vector<Point>& __my_chunk) {
    auto neighbours = this->neighbours();
    std::vector<std::vector<Point> > result;

    result.push_back(__my_chunk);

    for (const auto &neighbour : neighbours) {
        int pn = neighbour.first + neighbour.second * this->hor;

        this->send_neighbour(__my_chunk, neighbour.first, neighbour.second, pn * this->processes_count + process_number);
    }

    for (const auto &neighbour : neighbours) {
        int pn = neighbour.first + neighbour.second * this->hor;

        auto r = this->receive_neighbour(neighbour.first, neighbour.second, process_number * this->processes_count + pn);

        result.push_back(r);

    }

    return result;
}

void Sender::send_neighbour(const std::vector<Point>& __my_chunk, const int& x, const int& y, const int& tag) {
    int pn = x + y * this->hor;
    int size = __my_chunk.size();
    MPI_Send((void*) &size, 1, MPI_INT, pn, tag * 2, MPI_COMM_WORLD);

    if (size == 0) {
        return;
    }

    MPI_Send((void*) &__my_chunk.front(), size, Point::mpi_type, pn, tag * 2 + 1, MPI_COMM_WORLD);
}

std::vector<Point> Sender::receive_neighbour(const int& x, const int& y, const int& tag) {
    std::vector<Point> result;

    int pn = x + y * this->hor;
    int size;
    MPI_Status status;

    MPI_Recv((void*) &size, 1, MPI_INT, pn, tag * 2, MPI_COMM_WORLD, &status);

    if (size == 0) {
        return result;
    }

    result.resize(size);

    MPI_Recv((void*) &result.front(), size, Point::mpi_type, pn, tag * 2 + 1, MPI_COMM_WORLD, &status);

    return result;
}


std::vector<std::pair<int, int> > Sender::neighbours() {
    std::vector<std::pair<int, int> > result;

    const int part_x = process_number % this->hor;
    const int part_y = process_number / this->hor;

    int i = this->hor >= 3 ? -1 : 0;
    int max_i = this->hor >= 2 ? 1 : 0;

    for (; i <= max_i; i++) {
        int j = this->ver >= 3 ? -1 : 0;
        int max_j = this->ver >= 2 ? 1 : 0;

        for (; j <= max_j; j++) {
            if ((i != 0) || (j != 0)) {
                result.push_back(
                    std::make_pair(
                        positive_module(part_x + i, this->hor),
                        positive_module(part_y + j, this->ver)
                    )
                );
            }
        }
    }
    return result;
};

std::vector<Point> Sender::sent_initial(const std::vector<Point>& input) {
    if (alg == ALL) {
        return this->sent_initial_all(input);
    } else {
        // The strategy
        // Send from 0 to all what need to be sent
        // Send from all to their neighbours what is needed to be sent

        std::vector<Point> points;
        std::vector<int> sizes;

        if (this->process_number == 0) {
            chunks_t all_chunks;
            all_chunks = split(input, this->hor, this->ver);
            sizes.resize(this->processes_count);
            for (int j = 0; j < this->hor; j++) {
                for (int k = 0; k < this->ver; k++) {
                    sizes[j + k * hor] = all_chunks[j + k * this->hor].size();
                    points.insert(points.end(), all_chunks[j + k * this->hor].begin(), all_chunks[j + k * this->hor].end());
                }
            }
            points = flatten(all_chunks);
        }

        int my_size;

        MPI_Scatter((void *) &sizes.front(), 1, MPI_INT, &my_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // now we need to scatter initial

        std::vector<Point> receive;
        receive.resize(my_size);

        std::vector<int> dist;
        dist.resize(this->processes_count);
        if (process_number == 0) {
            dist[0] = 0;
            for (int i = 1; i < dist.size(); i++) {
                dist[i] = dist[i - 1] + sizes[i - 1];
            }
        }

        MPI_Scatterv((void*) &points.front(), &sizes.front(), &dist.front(), Point::mpi_type,
                     (void*) &receive.front(), my_size, Point::mpi_type, 0, MPI_COMM_WORLD);

        // now we need to send from source to neighbours

        auto x = this->distribute_neighbours(receive);

        return flatten(x);
    }
}

std::vector<Point> Sender::redistribute(std::vector<Point>& __data) {
    if (alg == AlgorithmVersion::ALL) {
        int m_err;

        int my_size[] = { static_cast<int>(__data.size()) };
        std::vector<int> sizes;
        sizes.resize(this->processes_count);

        m_err = MPI_Allgather((void*) &my_size[0], 1, MPI_INT,
                      (void*) &sizes.front(), 1, MPI_INT, MPI_COMM_WORLD);

        if (m_err != 0) {
            handle_error("sending data sizes");
        }

        std::vector<Point> result;
        std::vector<int> disp;
        disp.resize(this->processes_count);

        disp[0] = 0;

        for (int i = 1; i < disp.size(); i++) {
            disp[i] = disp[i - 1] + sizes[i - 1];
        }

        result.resize(this->gal_1_stars + this->gal_2_stars);

        m_err = MPI_Allgatherv((void*) &__data.front(), __data.size(), Point::mpi_type,
                               (void*) &result.front(), &sizes.front(), &disp.front(), Point::mpi_type, MPI_COMM_WORLD);

        if (m_err != 0) {
            handle_error("sending data");
        }

        return result;
    } else if (alg == AlgorithmVersion::NEIGHBOUR) {
        PRINTF_FL("DOWN WITH p=%d", this->process_number);
        handle_error("Not implemented yet.");
    } else {
        handle_error("Not implemented yet.");
    }
}

std::vector<int> Sender::distribute_new_sizes(std::vector<int>& __sizes) {
    std::vector<int> result;
    result.resize(this->processes_count);

    MPI_Alltoall((void*) &__sizes.front(), 1, MPI_INT, (void*) &result.front(), 1, MPI_INT, MPI_COMM_WORLD);

    for (const auto &s : result) {
        PRINTF_FL("p=%d s=%d", this->process_number, s);
    }

    return result;
}