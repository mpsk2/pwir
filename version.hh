#ifndef COLLISIONS_VERSION_HH
#define COLLISIONS_VERSION_HH

enum AlgorithmVersion {
    ALL,
    NEIGHBOUR,
    SELF
};

#ifdef ALG_V

#if ALG_V == 2
constexpr AlgorithmVersion alg = NEIGHBOUR;
#elif ALG_V == 3
constexpr AlgorithmVersion alg = SELF;
#else
constexpr AlgorithmVersion alg = ALL;
#endif

#else
constexpr AlgorithmVersion alg = ALL;
#endif

#endif //COLLISIONS_VERSION_HH
