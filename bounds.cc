#include "bounds.hh"
#include "errors.hh"

bounds_t bounds(const std::vector<Point>& __coords) {
    printf("\n\n\n%d\n\n", __coords.size());
    Point::coord_t minimum_x = __coords[0].x, minimum_y = __coords[0].y, maximum_x = __coords[0].x, maximum_y = __coords[0].y;
    for (auto coord : __coords) {
        minimum_x = std::min(minimum_x, coord.x);
        minimum_y = std::min(minimum_y, coord.x);
        maximum_x = std::max(maximum_x, coord.y);
        maximum_y = std::max(maximum_y, coord.y);
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
    Point::coord_t x = std::get<1>(__bounds) - std::get<0>(__bounds);
    Point::coord_t y = std::get<3>(__bounds) - std::get<2>(__bounds);

    x /= __max_x;
    y /= __max_y;

    return std::make_tuple(
            x * __x,
            std::min(x * (__x + 1), std::get<1>(__bounds)),
            y * __y,
            std::min(y * (__y + 1), std::get<3>(__bounds))
    );
}

bounds_t bounds_neighbour(const bounds_t& __bounds, const int& __x, const int& __y, const int& __max_x,
                          const int& __max_y, const int& __dx, const int& __dy) {
    auto x = (__x + __max_x + __dx) % __max_x;
    auto y = (__y + __max_y + __dy) % __max_y;

    return my_bounds(__bounds, x, y, __max_x, __max_y);
}

std::vector<bounds_t> area_bounds(const bounds_t& __bounds, const int& __x, const int& __y, const int& __max_x,
                                  const int& __max_y) {
    std::vector<bounds_t> result;

    auto i = __max_x >= 3 ? -1 : 0;
    auto max_i = __max_x >= 2 ? 1 : 0;

    for (; i <= max_i; i++) {
        auto j = __max_y >= 3 ? -1 : 0;
        auto max_j = __max_y >= 2 ? 1 : 0;

        for (; j <= max_j; j++) {
            result.push_back(bounds_neighbour(
                    __bounds,
                    __x,
                    __y,
                    __max_x,
                    __max_y,
                    i,
                    j
            ));
        }
    }

    return result;
}

inline bool is_in_bounds(const bounds_t& __bounds, const Point::coord_t& __x, const Point::coord_t& __y) {
    return (
            (std::get<0>(__bounds) <= __x) &&
            (std::get<1>(__bounds) >= __x) &&
            (std::get<2>(__bounds) <= __y) &&
            (std::get<3>(__bounds) >= __y)
    );
}

std::vector<Point> my_chunk(const std::vector<Point>& __data, const bounds_t& __bounds) {
    std::vector<Point> result;
    for (auto p : __data) {
        if (is_in_bounds(__bounds, p.x, p.y)) {
            result.push_back(p);
        }
    }

    return result;
}

chunks_t split(const std::vector<Point>& __data, const int& __hor, const int& __ver) {
    chunks_t result;

    result.resize(__hor);

    for (int i = 0; i < __hor; i++) {
        result[i].resize(__ver);
    }

    auto space = simulation_space(__data);

    for (int i = 0; i < __hor; i++) {
        for (int j = 0; j < __ver; j++) {
            auto mb = my_bounds(space, i, j, __hor, __ver);
            result[i][j] = my_chunk(__data, mb);
        }
    }

    return result;
}