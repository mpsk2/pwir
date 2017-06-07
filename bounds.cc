#include <cmath>
#include "PMISerializable.hh"
#include "Point.hh"
#include "bounds.hh"
#include "errors.hh"

bounds_t bounds(const std::vector<Point>& __coords) {
    Point::coord_t minimum_x = __coords[0].x, minimum_y = __coords[0].y, maximum_x = __coords[0].x, maximum_y = __coords[0].y;
    for (const auto &coord : __coords) {
        if (coord.x < minimum_x) {
            minimum_x = coord.x;
        }
        if (coord.y < minimum_y) {
            minimum_y = coord.y;
        }
        if (coord.x > maximum_x) {
            maximum_x = coord.x;
        }
        if (coord.y > maximum_y) {
            maximum_y = coord.y;
        }
    }
    return std::make_tuple(minimum_x, maximum_x, minimum_y, maximum_y);
}

bounds_t simulation_space(const std::vector<Point>& __coords) {
    auto b = bounds(__coords);
    return simulation_space(b);
}

bounds_t simulation_space(const bounds_t& __bounds) {
    Point::coord_t diff_x, diff_y;
    diff_x = std::get<1>(__bounds) - std::get<0>(__bounds);
    diff_y = std::get<3>(__bounds) - std::get<2>(__bounds);

    diff_x /= 2;
    diff_y /= 2;

    return std::make_tuple(
            std::get<0>(__bounds) - diff_x,
            std::get<1>(__bounds) + diff_x,
            std::get<2>(__bounds) - diff_y,
            std::get<3>(__bounds) + diff_y
    );
}

bounds_t my_bounds(const bounds_t& __bounds, const int& __x, const int& __y, const int& __max_x, const int& __max_y) {
    const Point::coord_t min_x = std::get<0>(__bounds);
    const Point::coord_t min_y = std::get<2>(__bounds);

    Point::coord_t x = std::get<1>(__bounds) - min_x;
    Point::coord_t y = std::get<3>(__bounds) - min_y;

    x /= __max_x;
    y /= __max_y;

    return std::make_tuple(
            x * __x + min_x,
            std::min(x * (__x + 1) + min_x, std::get<1>(__bounds)),
            y * __y + min_y,
            std::min(y * (__y + 1) + min_y, std::get<3>(__bounds))
    );
}

bounds_t bounds_neighbour(const bounds_t& __bounds, const int& __x, const int& __y, const int& __max_x,
                          const int& __max_y, const int& __dx, const int& __dy) {
    auto x = (__x + __max_x + __dx) % __max_x;
    auto y = (__y + __max_y + __dy) % __max_y;

    return my_bounds(__bounds, x, y, __max_x, __max_y);
}

inline bool is_in_bounds(const bounds_t& __bounds, const Point::coord_t& __x, const Point::coord_t& __y) {
    return (
            (std::get<0>(__bounds) <= __x) &&
            (std::get<1>(__bounds) > __x) &&
            (std::get<2>(__bounds) <= __y) &&
            (std::get<3>(__bounds) > __y)
    );
}

std::vector<Point> my_chunk(const std::vector<Point>& __data, const bounds_t& __bounds) {
    std::vector<Point> result;
    for (const auto &p : __data) {
        if (is_in_bounds(__bounds, p.x, p.y)) {
            result.push_back(p);
        }
    }

    return result;
}

chunks_t split(std::vector<Point>& __data, const int& __hor, const int& __ver, const bounds_t& __space) {
    chunks_t result;

    result.resize(__hor * __ver);

    for(auto &p : __data) {
        auto cell = flat_cell(p.x, p.y, __space, __hor, __ver);
        result[cell.first + cell.second * __hor].push_back(p);
    }
    return result;
}

std::pair<int, int> flat_cell(Point::coord_t& __x, Point::coord_t& __y, const bounds_t& __bounds, const int& __hor, const int& __ver) {
    const Point::coord_t x = std::get<1>(__bounds) - std::get<0>(__bounds);
    const Point::coord_t y = std::get<3>(__bounds) - std::get<2>(__bounds);

    const Point::coord_t nx = __x - std::get<0>(__bounds);
    const Point::coord_t ny = __y - std::get<2>(__bounds);

    Point::coord_t xc = std::floor(nx * __hor / x);
    Point::coord_t yc = std::floor(ny * __ver / y);

    if (xc == __hor) {
        --xc;
        __x = std::get<0>(__bounds);
    }

    if (yc == __ver) {
        yc = 0;
        __y = std::get<2>(__bounds);
    }

    return std::make_pair(xc, yc);
}