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