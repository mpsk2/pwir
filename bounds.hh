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

typedef std::vector<std::vector<Point> > chunks_t;

chunks_t split(std::vector<Point>& __data, const int& __hor, const int& __ver, const bounds_t& __bn);
std::pair<int, int> flat_cell(Point::coord_t& __x, Point::coord_t& __y, const bounds_t& __bounds, const int& __hor, const int& __ver);

#endif //COLLISIONS_BOUNDS_HH
