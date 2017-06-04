#include <sstream>

#include "Point.hh"

const std::string Point::str() const noexcept {
    std::stringstream ss;

    ss << "POS(id=";
    ss << this->id;
    ss << ", mass";
    ss << this->mass;
    ss << ", x";
    ss << this->x;
    ss << ", y";
    ss << this->y;
    ss << ", speed_x";
    ss << this->speed_x;
    ss << ", speed_y";
    ss << this->speed_y;
    ss << ", acceleration_x";
    ss << this->acceleration_x;
    ss << ", acceleration_y";
    ss << this->acceleration_y;
    ss << ")";

    return ss.str();
}

const inline char* Point::c_str() const noexcept {
    return this->str().c_str();
}

std::ostream& operator<< (std::ostream& stream, const Point& point) {
    stream << point.str();
    return stream;
}

constexpr MPI_Datatype Point::types[] = {
        MPI_DOUBLE, // x
        MPI_DOUBLE, // y
        MPI_DOUBLE, // speed x
        MPI_DOUBLE, // speed y
        MPI_DOUBLE, // acceleration x
        MPI_DOUBLE, // acceleration y
        MPI_DOUBLE, // mass
        MPI_INT,    // id
};

const MPI_Aint Point::offsets[] = {
        offsetof(Point, x),
        offsetof(Point, y),
        offsetof(Point, speed_x),
        offsetof(Point, speed_y),
        offsetof(Point, acceleration_x),
        offsetof(Point, acceleration_y),
        offsetof(Point, mass),
        offsetof(Point, id),
};

MPI_Datatype Point::mpi_type;

void Point::create_type() {
    MPI_Type_create_struct(Point::items_count, &Point::block_lengths().front(), Point::offsets, Point::types, &Point::mpi_type);
    MPI_Type_commit(&Point::mpi_type);
}
