#ifndef COLLISIONS_ALGORITHM_HH
#define COLLISIONS_ALGORITHM_HH

#include <vector>
#include <limits>

constexpr Point::coord_t Gc = 155893.597f;
constexpr Point::coord_t ignore_distance = 1e-5f;
constexpr Point::coord_t force_threshold = std::numeric_limits<Point::coord_t>::max() / 2.0;

typedef std::pair<Point::coord_t, Point::coord_t> acceleration_t;

acceleration_t calc_acceleration(const std::vector<Point>& __data, const Point::coord_t& x, const Point::coord_t& y, const Point::coord_t& __mass);
Point step_target(const std::vector<Point>& __data, __const Point& __target, const Point::coord_t& __time_diff);
std::vector<Point> step_chunk(const std::vector<Point>& __to_calculate, const std::vector<Point>& __all, const Point::coord_t& __time_diff);

std::vector<Point> borders(std::vector<Point>& __data, const bounds_t& __bounds);

#endif //COLLISIONS_ALGORITHM_HH
