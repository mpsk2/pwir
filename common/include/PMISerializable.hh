#ifndef COLLISIONS_PMISERIALIZABLE_HH
#define COLLISIONS_PMISERIALIZABLE_HH

#include <array>
#include <mpi.h>

template<int N>
class MPISerializable {
public:
    static constexpr int items_count = N;
    static std::array<int, N> block_lengths() {
        std::array<int, N> result;
        std::fill_n(result.begin(), N, 1);
        return result;
    }
};

#endif //COLLISIONS_PMISERIALIZABLE_HH
