#ifndef COLLISIONS_BOUNDS_HH
#define COLLISIONS_BOUNDS_HH

#include <algorithm>
#include <vector>
#include <tuple>
#include "Point.hh"

typedef std::tuple<double, double, double, double> bounds_t;

bounds_t bounds(const std::vector<Point>&);
bounds_t simulation_space(const std::vector<Point>&);
bounds_t simulation_space(const bounds_t&);
bounds_t my_bounds(const bounds_t&, const int&, const int&, const int&, const int&);
constexpr inline bool is_in_bounds(const bounds_t&, const double&, const double&);

#endif //COLLISIONS_BOUNDS_HH
