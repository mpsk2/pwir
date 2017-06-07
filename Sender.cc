#include <mpi.h>
#include <set>
#include <iterator>
#include <sstream>
#include "bounds.hh"
#include "Sender.hh"
#include "errors.hh"
#include "version.hh"

Sender::Sender(const int& __process_number, const int& __processes_count, const int& __gal_1_stars,
       const int& __gal_2_stars, const bool& __verbose, const int& __hor, const int& __ver, const bounds_t& __bn) noexcept :
        process_number(__process_number),
        processes_count(__processes_count),
        gal_1_stars(__gal_1_stars),
        gal_2_stars(__gal_2_stars),
        verbose(__verbose),
        ver(__ver),
        hor(__hor),
        bn(__bn) {
    this->my_neighbours = this->neighbours();

    if (alg == SELF) {
        this->self_disp.resize(this->processes_count);
        this->self_sizes.resize(this->processes_count);

        const int stars = this->gal_1_stars + this->gal_2_stars;

        const int additional = stars % this->processes_count;
        const int base_each = stars / this->processes_count;

        std::fill_n(this->self_sizes.begin(), additional, base_each + 1);
        std::fill(this->self_sizes.begin() + additional, this->self_sizes.end(), base_each);

        this->self_disp[0] = 0;
        for (int i = 1; i < this->processes_count; i++) {
            this->self_disp[i] = this->self_disp[i - 1] + this->self_disp[i - 1];
        }
    }
}

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
    std::vector<std::vector<Point> > result;

    result.push_back(__my_chunk);

    for (const auto &neighbour : this->my_neighbours) {
        int pn = neighbour.first + neighbour.second * this->hor;
        int _tag = this->base_tag(pn, this->process_number, INITIAL);

        this->send_neighbour(__my_chunk, neighbour.first, neighbour.second, _tag);
    }

    for (const auto &neighbour : this->my_neighbours) {
        int pn = neighbour.first + neighbour.second * this->hor;
        int _tag = this->base_tag(this->process_number, pn, INITIAL);

        auto r = this->receive_neighbour(neighbour.first, neighbour.second, _tag);

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
Sender::neighbours_t Sender::neighbours(const int &__target) {
    Sender::neighbours_t result;

    const int part_x = __target % this->hor;
    const int part_y = __target / this->hor;

    int i = this->hor >= 3 ? -1 : 0;
    int max_i = this->hor >= 2 ? 1 : 0;

    for (; i <= max_i; i++) {
        int j = this->ver >= 3 ? -1 : 0;
        int max_j = this->ver >= 2 ? 1 : 0;

        for (; j <= max_j; j++) {
            result.push_back(
                std::make_pair(
                    positive_module(part_x + i, this->hor),
                    positive_module(part_y + j, this->ver)
                )
            );
        }
    }
    return result;
}

Sender::neighbours_t Sender::neighbours() {
    Sender::neighbours_t result;

    const int part_x = this->process_number % this->hor;
    const int part_y = this->process_number / this->hor;

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

std::vector<Point> Sender::sent_initial_neighbours(std::vector<Point>& input) {
    // The strategy
    // Send from 0 to all what need to be sent
    // Send from all to their neighbours what is needed to be sent

    std::vector<Point> points;
    std::vector<int> sizes;

    if (this->process_number == 0) {
        chunks_t all_chunks;
        all_chunks = split(input, this->hor, this->ver, this->bn);
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
    if (this->process_number == 0) {
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

std::vector<Point> Sender::sent_initial(std::vector<Point>& input) {
    std::vector<Point> result;

    if (alg == AlgorithmVersion::ALL) {
        result = this->sent_initial_all(input);
    } else if (alg == AlgorithmVersion::NEIGHBOUR) {
        result = this->sent_initial_neighbours(input);
    } else {
        result = this->sent_initial_all(input);
    }

    return result;
}

std::vector<Point> Sender::redistribute_all(std::vector<Point>& __data) {
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
}

std::vector<Point> Sender::redistribute(std::vector<Point>& __data) {
    std::vector<Point> result;

    if (alg == AlgorithmVersion::ALL) {
        result = this->redistribute_all(__data);
    } else if (alg == AlgorithmVersion::NEIGHBOUR) {
        result = this->redistribute_chunks(__data);
    } else {
        result = this->redistribute_self(__data);
    }

    return result;
}


std::vector<int> Sender::gather_sizes(const chunks_t& __data) {
    std::vector<int> result;
    result.resize(this->processes_count * this->processes_count);
    std::vector<int> sending;
    sending.resize(this->processes_count);

    for (int i = 0; i < this->processes_count; i++) {
        sending[i] = __data[i].size();
    }

    MPI::COMM_WORLD.Allgather((void*) &sending.front(), this->processes_count, MPI::INT, (void*) &result.front(), this->processes_count, MPI::INT);

    return result;
}

std::vector<Point> Sender::redistribute_chunks(std::vector<Point>& __data) {
    std::vector<Point> result;
    auto chunks = split(__data, this->hor, this->ver, this->bn);

    auto sizes = this->gather_sizes(chunks);

    std::vector<std::vector<Point> > sub_res;
    sub_res.push_back(__data);

    for (int cn = 0; cn < this->processes_count; cn++) {
        if (chunks[cn].size() == 0) {
            continue;
        }

        auto neighs = this->neighbours(cn);

        for (int neigh = 0; neigh < neighs.size(); neigh++) {
            auto pn = neighs[neigh].first + neighs[neigh].second * this->hor;

            if (pn == this->process_number) {
                continue;
            }

            auto tag = this->base_tag(this->process_number, pn, REDISTRIBUTE) * this->processes_count + cn;

            MPI::COMM_WORLD.Send((void*) &chunks[cn].front(), chunks[cn].size(), Point::mpi_type, pn, tag);
        }
    }

    for (const auto &n : this->neighbours(this->process_number)) {
        auto cn = n.first + n.second * this->hor;
        for (int i = 0; i < this->processes_count; i++) {
            if (i == this->process_number) {
                continue;
            }
            auto sn = cn + i * this->processes_count;
            if (sizes[sn] == 0) {
                continue;
            }
            auto tag = this->base_tag(i, this->process_number, REDISTRIBUTE) * this->processes_count + cn;

            std::vector<Point> p;
            p.resize(sizes[sn]);
            MPI::COMM_WORLD.Recv((void*) &p.front(), p.size(), Point::mpi_type, i, tag);

            sub_res.push_back(p);
        }
    }

    return flatten(sub_res);
}

int Sender::base_tag(const int& __src, const int& __dest,  const Sender::OP& operation) {
    return __src + __dest * this->processes_count + operation * this->processes_count * processes_count;
}

std::vector<Point> Sender::gather_all_at_root(std::vector<Point>& __data, const int& __sum) {
    // that method is important for --verbose / -v and for final result.

    std::vector<int> sizes;
    sizes.resize(this->processes_count);
    std::vector<Point> result;

    auto size = __data.size();

    MPI_Gather((void*) &size, 1, MPI_INT, (void*) &sizes.front(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> disp;
    disp.resize(this->processes_count);

    if (this->processes_count == 0) {
        for (int i = 0; i < this->processes_count - 1; i++) {
            disp[i+ 1] = (disp[i] + sizes[i]);
        }

        result.resize(__sum);
    }

    if (this-> process_number == 0) {
        std::vector<std::vector<Point> > subresult;
        subresult.resize(this->processes_count);
        subresult[0] = __data;
        for (int i = 1; i < this->processes_count; i++) {
            if (sizes[i] != 0) {
                subresult[i].resize(sizes[i]);
                MPI::COMM_WORLD.Recv((void*) &subresult[i].front(), sizes[i], Point::mpi_type, i, this->tag_gather_all);
            }

        }
        result = flatten(subresult);
    } else {
        if (size != 0) {
            MPI::COMM_WORLD.Send((void *) &__data.front(), size, Point::mpi_type, 0, this->tag_gather_all);
        }
    }

    return result;
}

std::vector<Point> Sender::redistribute_self(const std::vector<Point>& __chunks) {
    std::vector<Point> result;
    result.resize(this->gal_1_stars + this->gal_2_stars);

    MPI::COMM_WORLD.Allgatherv(
            (void*) &__chunks.front(), __chunks.size(), Point::mpi_type,
            (void*) &result.front(), &this->self_sizes.front(), &this->self_disp.front(), Point::mpi_type);

    return result;
}