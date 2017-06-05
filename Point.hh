#ifndef COLLISIONS_POINT_HH
#define COLLISIONS_POINT_HH

#include <string>
#include <ostream>
#include <mpi.h>
#include <stddef.h>
#include <vector>
#include "PMISerializable.hh"

class Point : MPISerializable<8> {
public:
    typedef float coord_t;
    coord_t x;
    coord_t y;
    coord_t speed_x;
    coord_t speed_y;
    coord_t acceleration_x;
    coord_t acceleration_y;
    coord_t mass;
    int id;

    Point(const coord_t& __x,
          const coord_t& __y,
          const coord_t& __speed_x,
          const coord_t& __speed_y,
          const coord_t& __acceleration_x,
          const coord_t& __acceleration_y,
          const coord_t& __mass,
          const int& __id
    ) noexcept : x(__x),
        y(__y),
        speed_x(__speed_x),
        speed_y(__speed_y),
        acceleration_x(__acceleration_x),
        acceleration_y(__acceleration_y),
        mass(__mass),
        id(__id) {}

    Point(const coord_t& __x,
          const coord_t& __y,
          const coord_t& __speed_x,
          const coord_t& __speed_y,
          const coord_t& __mass,
          const int& __id
    ) noexcept : Point(__x, __y, __speed_x, __speed_y, 0, 0, __mass, __id) {}
    Point() = default;

    const std::string str() const noexcept;
    friend std::ostream& operator<< (std::ostream& stream, const Point& coord);

    void fill_acceleration(std::vector<Point>& __data);

    const static MPI_Datatype types[];
    const static MPI_Aint offsets[];
    static MPI_Datatype mpi_type;

    static void create_type();

    static void fill_accelerations(std::vector<Point>& __data);
};

#endif
