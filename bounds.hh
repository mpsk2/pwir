#ifndef COLLISIONS_BOUNDS_HH
#define COLLISIONS_BOUNDS_HH

#include <algorithm>
#include <vector>
#include <tuple>
#include "Point.hh"

typedef std::tuple<Point::coord_t, Point::coord_t, Point::coord_t, Point::coord_t> bounds_t;

bounds_t bounds(const std::vector<Point>&);
bounds_t simulation_space(const std::vector<Point>&);
bounds_t simulation_space(const bounds_t&);
bounds_t my_bounds(const bounds_t&, const int&, const int&, const int&, const int&);
inline bool is_in_bounds(const bounds_t&, const Point::coord_t&, const Point::coord_t&);

std::vector<Point> my_chunk(const std::vector<Point>& __data, const bounds_t& __bounds);

#endif //COLLISIONS_BOUNDS_HH
