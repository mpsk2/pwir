#ifndef COLLISIONS_FILEHEADER_HH
#define COLLISIONS_FILEHEADER_HH

#include <mpi.h>
#include "PMISerializable.hh"

class FileHeader : public MPISerializable<5> {
public:
    int stars_number;
    int number;
    double speed_x;
    double speed_y;
    double mass;
    FileHeader() = default;
    FileHeader(const int&);
    FileHeader(const int&, const double&, const double&, const double&, const int&);


    void broadcast();
    static FileHeader& receive();

    const static MPI_Datatype types[];
    const static MPI_Aint offsets[];
    static MPI_Datatype mpi_type;

    static void create_type();

    virtual ~FileHeader() {}
};

#endif //COLLISIONS_FILEHEADER_HH
