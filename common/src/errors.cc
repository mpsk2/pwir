#include <iostream>
#include <mpi.h>

#include "errors.hh"

static void common_handle_error() {
    MPI_Finalize();
    exit(1);
}

void handle_error() {
    std::cerr << "Error without msg.\n";
    common_handle_error();
}

void handle_error(std::string msg) {
    std::cerr << "Error with msg=\"" + msg + "\".\n";
    common_handle_error();
}