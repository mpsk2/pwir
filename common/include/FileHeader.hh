#ifndef COLLISIONS_FILEHEADER_HH
#define COLLISIONS_FILEHEADER_HH

#include <string>
#include <mpi.h>
#include "PMISerializable.hh"
#include "Point.hh"
#include "../../bounds.hh"

class FileHeader : public MPISerializable<9> {
public:
    int stars_number;
    int number;
    Point::coord_t speed_x;
    Point::coord_t speed_y;
    Point::coord_t mass;
    Point::coord_t bound_up;
    Point::coord_t bound_down;
    Point::coord_t bound_left;
    Point::coord_t bound_right;
    FileHeader() = default;
    FileHeader(const int&);
    FileHeader(const int&, const Point::coord_t&, const Point::coord_t&, const Point::coord_t&, const int&);

    void broadcast();
    static FileHeader& receive();
    std::string str();

    bounds_t borders() { return std::make_tuple(this->bound_left, this->bound_right, this->bound_down, this->bound_up); }

    static MPI_Datatype types[];
    static MPI_Aint offsets[];
    static MPI_Datatype mpi_type;

    static void create_type();

    virtual ~FileHeader() {}
};

#endif //COLLISIONS_FILEHEADER_HH
