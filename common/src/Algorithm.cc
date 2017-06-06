#include <cmath>
#include <tuple>
#include <iostream>
#include "Algorithm.hh"
#include "errors.hh"
#include "../../bounds.hh"

std::pair<Point::coord_t, Point::coord_t> calc_acceleration(const std::vector<Point>& __data, const Point::coord_t& x, const Point::coord_t& y) {
    Point::coord_t acceleration_x = 0;
    Point::coord_t acceleration_y = 0;
    Point::coord_t distance_x;
    Point::coord_t distance_y;

    Point::coord_t tmp;
    Point::coord_t distance;

    for (auto d : __data) {
        distance_x = x - d.x;
        distance_y = y - d.y;

        distance = std::sqrt(std::pow(distance_x, 2) + std::pow(distance_y, 2));

        if (distance <= ignore_distance) {
            continue;
        }

        tmp = std::pow(distance, 3);
        acceleration_x -= d.mass / tmp;
        acceleration_y -= d.mass / tmp;
    }

    acceleration_x *= Gc;
    acceleration_y *= Gc;

    return std::make_pair(acceleration_x, acceleration_y);
};


Point step_target(const std::vector<Point>& __data, const Point& __target, const Point::coord_t& __time_diff) {
    Point::coord_t r_x;
    Point::coord_t r_y;

    const Point::coord_t __time_2 = std::pow(__time_diff, 2);

    r_x = __target.x + __target.speed_x * __time_diff + __target.acceleration_x * __time_2 / 2;
    r_y = __target.y + __target.speed_y * __time_diff + __target.acceleration_y * __time_2 / 2;

    return Point(
      r_x,
      r_y,
      __target.speed_x,
      __target.speed_y,
      __target.acceleration_x,
      __target.acceleration_y,
      __target.mass,
      __target.id
    );
}

Point change_speed(const std::vector<Point>& __data, Point& __target, const Point::coord_t& __time_diff) {
    Point::coord_t speed_x;
    Point::coord_t speed_y;

    auto acceleration = calc_acceleration(__data, __target.x, __target.y);

    speed_x = __target.speed_x + (acceleration.first + __target.acceleration_x) * __time_diff / 2;
    speed_y = __target.speed_y + (acceleration.second + __target.acceleration_y) * __time_diff / 2;

    // printf("%f %f %f %f %f\n", speed_x, __target.speed_x, acceleration.first, __target.acceleration_x, __time_diff);

    return Point(
            __target.x,
            __target.y,
            speed_x,
            speed_y,
            acceleration.first,
            acceleration.second,
            __target.mass,
            __target.id
    );
}

std::vector<Point> step_chunk(const std::vector<Point>& __to_calculate, const std::vector<Point>& __all, const Point::coord_t& __time_diff) {
    std::vector<Point> result;

    for (auto p : __to_calculate) {
        result.push_back(step_target(__all, p, __time_diff));
    }

    for (int i = 0; i < result.size(); i++) {
        result[i] = change_speed(__all, result[i], __time_diff);
    }

    return result;
}

Point remap(Point& __p, const bounds_t& __bounds) {
    Point::coord_t diff_x = std::get<1>(__bounds) - std::get<0>(__bounds);
    Point::coord_t diff_y = std::get<3>(__bounds) - std::get<2>(__bounds);

    Point::coord_t diff_x_p = std::get<0>(__bounds) - __p.x;
    Point::coord_t diff_y_p = std::get<2>(__bounds) - __p.y;

    if (diff_x_p < 0) {
        diff_x = -diff_x;
    }

    if (diff_y_p < 0) {
        diff_y = -diff_y;
    }

    while (std::get<1>(__bounds) < __p.x || std::get<0>(__bounds) > __p.x) {
        __p.x += diff_x;
    }
    while (std::get<3>(__bounds) < __p.y || std::get<2>(__bounds) > __p.y) {
        __p.y += diff_y;
    }

    printf("remap %f %f %f %f\n", diff_x, diff_y, diff_x_p, diff_y_p);
    return __p;
}

std::vector<Point> borders(std::vector<Point>& __data, const bounds_t& __bounds) {
    for (int i = 0; i < __data.size(); i++) {
        if ((std::get<0>(__bounds) > __data[i].x) ||
            (std::get<1>(__bounds) < __data[i].x) ||
            (std::get<2>(__bounds) > __data[i].y) ||
            (std::get<3>(__bounds) < __data[i].y)
                ) {
            __data[i] = remap(__data[i], __bounds);
        }
    }
    return __data;
}