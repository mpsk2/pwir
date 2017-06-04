#include <stddef.h>
#include "FileHeader.hh"

FileHeader::FileHeader(const int& __number) : FileHeader(0, 0, 0, 0, __number) {}

FileHeader::FileHeader(const int& __stars_number, const double& __speed_x, const double& __speed_y, const double& __mass, const int& __number) :
        stars_number(__stars_number),
        speed_x(__speed_x),
        speed_y(__speed_y),
        mass(__mass),
        number(__number) {
}

const MPI_Aint FileHeader::offsets[] = {
        offsetof(FileHeader, number),
        offsetof(FileHeader, stars_number),
        offsetof(FileHeader, speed_x),
        offsetof(FileHeader, speed_y),
        offsetof(FileHeader, mass)
};

const MPI_Datatype FileHeader::types[] = {
        MPI_INT,
        MPI_INT,
        MPI_DOUBLE,
        MPI_DOUBLE,
        MPI_DOUBLE
};

MPI_Datatype FileHeader::mpi_type;

void FileHeader::create_type() {
    int block_lengths[FileHeader::items_count];
    std::fill_n(block_lengths, FileHeader::items_count, 1);

    MPI_Type_create_struct(FileHeader::items_count, block_lengths, FileHeader::offsets, FileHeader::types, &FileHeader::mpi_type);
    MPI_Type_commit(&FileHeader::mpi_type);
}

void FileHeader::broadcast() {
    MPI_Bcast(this, 1, this->mpi_type, 0, MPI_COMM_WORLD);
}

FileHeader& FileHeader::receive() {
    FileHeader* res = new FileHeader;
    MPI_Bcast(res, 1, mpi_type, 0, MPI_COMM_WORLD);
    return *res;
}