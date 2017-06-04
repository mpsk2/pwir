#ifndef COLLISIONS_POINT_HH
#define COLLISIONS_POINT_HH

#include <string>
#include <ostream>
#include <mpi.h>
#include <stddef.h>
#include "PMISerializable.hh"

class Point : MPISerializable<8> {
public:
    double x;
    double y;
    double speed_x;
    double speed_y;
    double acceleration_x;
    double acceleration_y;
    double mass;
    int id;

    Point(const double& __x,
          const double& __y,
          const double& __speed_x,
          const double& __speed_y,
          const double& __acceleration_x,
          const double& __acceleration_y,
          const double& __mass,
          const int& __id
    ) noexcept : x(__x),
        y(__y),
        speed_x(__speed_x),
        speed_y(__speed_y),
        acceleration_x(__acceleration_x),
        acceleration_y(__acceleration_y),
        mass(__mass),
        id(__id) {}

    Point(const double& __x,
          const double& __y,
          const double& __speed_x,
          const double& __speed_y,
          const double& __mass,
          const int& __id
    ) noexcept : Point(__x, __y, __speed_x, __speed_y, 0, 0, __mass, __id) {}
    Point() = default;

    const std::string str() const noexcept;
    const inline char* c_str() const noexcept ;
    friend std::ostream& operator<< (std::ostream& stream, const Point& coord);

    const static MPI_Datatype types[];
    const static MPI_Aint offsets[];
    static MPI_Datatype mpi_type;

    static void create_type();
};

#endif
