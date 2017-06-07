#include <cmath>
#include <tuple>
#include <iostream>
#include <map>
#include "PMISerializable.hh"
#include "Point.hh"
#include "bounds.hh"
#include "Algorithm.hh"
#include "errors.hh"

std::pair<Point::coord_t, Point::coord_t> calc_acceleration(const std::vector<Point>& __data, const Point::coord_t& x,
                                                            const Point::coord_t& y, const Point::coord_t& __mass) {
    Point::coord_t acceleration_x = 0;
    Point::coord_t acceleration_y = 0;
    Point::coord_t distance_x;
    Point::coord_t distance_y;

    Point::coord_t tmp;
    Point::coord_t distance;

    for (const auto &d : __data) {
        distance_x = x - d.x;
        distance_y = y - d.y;

        distance = std::sqrt(std::pow(distance_x, 2) + std::pow(distance_y, 2));

        if (distance <= ignore_distance) {
            continue;
        }

        tmp = std::pow(distance, 3);
        acceleration_x -= distance_x * d.mass / tmp;
        acceleration_y -= distance_y * d.mass / tmp;
    }

    acceleration_x *= Gc;
    acceleration_y *= Gc;

    if ( (acceleration_x * __mass >= force_threshold) || (acceleration_y * __mass >= force_threshold) ) {
        return std::make_pair(0, 0);
    }

    return std::make_pair(acceleration_x, acceleration_y);
};

std::vector<Point> step_chunk(const std::vector<Point>& __to_calculate, const std::vector<Point>& __all, const Point::coord_t& __time_diff) {
    std::vector<Point> result;

    std::map<int, std::pair<Point::coord_t, Point::coord_t> > new_points;
    const Point::coord_t td2 = std::pow(__time_diff, 2);

    for (const auto &target : __all) {
        // r(t + dt) = r(t) + v(t) * dt + 0.5 * a(t) * t ^ 2
        const Point::coord_t new_x = target.x + target.speed_x * __time_diff + target.acceleration_x * td2 / 2;
        const Point::coord_t new_y = target.y + target.speed_y * __time_diff + target.acceleration_y * td2 / 2;

        new_points[target.id] = std::make_pair(new_x, new_y);
    }


    for (int i = 0; i < __to_calculate.size(); i++) {
        const auto &target = __to_calculate[i];
        const auto &new_pos = new_points[target.id];

        Point::coord_t acceleration_x = 0;
        Point::coord_t acceleration_y = 0;

        // a(t, i) = F(t, i) / m(i)
        // F(t, i) = - sum(i =/= j, G * m(i) * m(j) * (r(i) - r(j)) / |r(i) - r(j)| ^ 3
        // below we calculate a(t + dt, i)
        for (const auto &other : __all) {
            if (target.id == other.id) {
                continue;
            }

            const Point::coord_t x_delta = new_pos.first - new_points[other.id].first;
            const Point::coord_t y_delta = new_pos.second - new_points[other.id].second;
            const Point::coord_t dist = std::sqrt(
                    std::pow(x_delta, 2) +
                    std::pow(y_delta, 2)
            );

            if (dist == 0) {
                continue;
            }

            const Point::coord_t triple_dist = std::pow(dist, 3);
            acceleration_x -= other.mass * x_delta / triple_dist;
            acceleration_y -= other.mass * y_delta / triple_dist;
        }

        acceleration_x *= Gc;
        acceleration_y *= Gc;

        if ((acceleration_x * target.mass > force_threshold) || (acceleration_y * target.mass > force_threshold)) {
            result.push_back(target);
        } else {
            // v(t + dt) = v(t) (a(t) + a(t + dt)) * dt
            const Point::coord_t new_speed_x = target.speed_x + __time_diff * (target.acceleration_x + acceleration_x) / 2;
            const Point::coord_t new_speed_y = target.speed_y + __time_diff * (target.acceleration_y + acceleration_y) / 2;

            result.push_back(Point(
                    new_pos.first,
                    new_pos.second,
                    new_speed_x,
                    new_speed_y,
                    acceleration_x,
                    acceleration_y,
                    target.mass,
                    target.id
            ));
        }
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

    while ((std::get<1>(__bounds) < __p.x) || (std::get<0>(__bounds) > __p.x)) {
        __p.x += diff_x;
    }
    while ((std::get<3>(__bounds) < __p.y) || (std::get<2>(__bounds) > __p.y)) {
        __p.y += diff_y;
    }
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