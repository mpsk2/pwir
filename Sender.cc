#include <mpi.h>
#include "Sender.hh"
#include "errors.hh"
#include "version.hh"

std::vector<Point> Sender::sent_initial(const std::vector<Point>& input) {
    if (alg == AlgorithmVersion::ALL) {
        int m_err;
        std::vector<Point> result;
        if (this->process_number == 0) {
            m_err = MPI_Bcast((void *) &input.front(), input.size(), Point::mpi_type, 0, MPI_COMM_WORLD);
            result = input;
        } else {
            result.resize(this->gal_1_stars + this->gal_2_stars);
            m_err = MPI_Bcast((void *) &result.front(), result.size(), Point::mpi_type, 0, MPI_COMM_WORLD);
        }
        if (m_err != 0) {
            handle_error("sending initial data");
        }

        return result;
    } else {
        handle_error("Not implemented yet.");
    }
}

std::vector<Point> Sender::redistribute(std::vector<Point>& __data) {
    if (alg == AlgorithmVersion::ALL) {
        int m_err;

        int my_size[] = { __data.size() };
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
    } else {
        handle_error("Not implemented yet.");
    }
}
