#include <mpi.h>
#include <set>
#include "bounds.hh"
#include "Sender.hh"
#include "errors.hh"
#include "version.hh"


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

std::vector<Point> Sender::distribute_chunks(const chunks_t& chunks) {
    // Now we should send sizes

    std::vector<int> receive_sizes;
    receive_sizes.resize(1);

    std::vector<int> send_sizes;
    if (process_number == 0) {
        send_sizes.resize(this->processes_count);
        for (int i = 0; i < chunks.size(); i++) {
            for (int j = 0; j < chunks[i].size(); j++) {
                send_sizes[i + j * this->hor] = chunks[i][j].size();
            }
        }
    }

    MPI_Scatter((void*) &send_sizes.front(), 1, MPI_INT, (void*) &receive_sizes.front(), 1,
                MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<Point> my_chunk;
    my_chunk.resize(receive_sizes[0]);

    std::vector<int> disp;

    if (process_number == 0) {
        disp.push_back(0);
        for (int i = 1; i < send_sizes.size(); i++) {
            disp.push_back(
                    disp[i - 1] + send_sizes[i - 1]
            );
        }
    }

    MPI_Scatterv((void*) &chunks.front(), &send_sizes.front(), &disp.front(), Point::mpi_type,
                 (void*) &my_chunk.front(), receive_sizes[0], Point::mpi_type, 0, MPI_COMM_WORLD);

    return my_chunk;
}

std::vector<Point> Sender::sent_initial(const std::vector<Point>& input) {
    return this->sent_initial_all(input);
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
