#include <stddef.h>
#include <sstream>
#include "FileHeader.hh"

FileHeader::FileHeader(const int& __number) : FileHeader(0, 0, 0, 0, __number) {}

FileHeader::FileHeader(const int& __stars_number, const Point::coord_t& __speed_x, const Point::coord_t& __speed_y, const Point::coord_t& __mass, const int& __number) :
        stars_number(__stars_number),
        speed_x(__speed_x),
        speed_y(__speed_y),
        mass(__mass),
        number(__number) {
}

MPI_Aint FileHeader::offsets[] = {
        offsetof(FileHeader, number),
        offsetof(FileHeader, stars_number),
        offsetof(FileHeader, speed_x),
        offsetof(FileHeader, speed_y),
        offsetof(FileHeader, bound_up),
        offsetof(FileHeader, bound_down),
        offsetof(FileHeader, bound_left),
        offsetof(FileHeader, bound_right),
        offsetof(FileHeader, mass)
};

MPI_Datatype FileHeader::types[] = {
        MPI_INT,
        MPI_INT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT
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

std::string FileHeader::str() {
    std::stringstream ss;

    ss << "GalHead ";
    ss << this->number;
    ss << " ";
    ss << this->stars_number;
    ss << " ";
    ss << this->speed_x;
    ss << " ";
    ss << this->speed_y;
    ss << " ";
    ss << this->mass;
    ss << " ";
    ss << this->bound_left;
    ss << " ";
    ss << this->bound_right;
    ss << " ";
    ss << this->bound_down;
    ss << " ";
    ss << this->bound_up;
    ss << "!";

    return ss.str();
}
