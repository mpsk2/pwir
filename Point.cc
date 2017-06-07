#include <sstream>
#include <iostream>
#include "PMISerializable.hh"
#include "Point.hh"
#include "bounds.hh"
#include "Algorithm.hh"

const std::string Point::str() const noexcept {
    std::stringstream ss;

    ss << "POS(id=";
    ss << this->id;
    ss << ", mass=";
    ss << this->mass;
    ss << ", x=";
    ss << this->x;
    ss << ", y=";
    ss << this->y;
    ss << ", speed_x=";
    ss << this->speed_x;
    ss << ", speed_y=";
    ss << this->speed_y;
    ss << ", acceleration_x=";
    ss << this->acceleration_x;
    ss << ", acceleration_y=";
    ss << this->acceleration_y;
    ss << ")";

    return ss.str();
}

std::ostream& operator<< (std::ostream& stream, const Point& point) {
    stream << point.str();
    return stream;
}

void Point::fill_acceleration(std::vector<Point>& __data) {
    auto acc = calc_acceleration(__data, this->x, this->y, this->mass);
    this->acceleration_x = acc.first;
    this->acceleration_y = acc.second;
}

MPI_Datatype Point::types[] = {
        Point::mpi_coord_t, // x
        Point::mpi_coord_t, // y
        Point::mpi_coord_t, // speed x
        Point::mpi_coord_t, // speed y
        Point::mpi_coord_t, // acceleration x
        Point::mpi_coord_t, // acceleration y
        Point::mpi_coord_t, // mass
        MPI_INT,    // id
};

MPI_Aint Point::offsets[] = {
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

void Point::fill_accelerations(std::vector<Point>& __data) {
    for (auto &p : __data) {
        p.fill_acceleration(__data);
    }
}