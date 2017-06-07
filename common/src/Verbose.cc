#include <fstream>
#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "Verbose.hh"
#include "errors.hh"

bool compare(Point p1, Point p2) {
    return p1.id < p2.id;
}

void write_file(std::vector<Point>& points, const int& first_len, const int& second_len, const bool& __final) {
    static int iteration = 0;

    std::sort(points.begin(), points.end(), compare);

    if (second_len + first_len != points.size()) {
        PRINTF_FL("%d %d %d", points.size(), second_len, first_len);
        for (const auto &p: points) {
            PRINTF_FL("GOT %s", p.str().c_str());
        }
        handle_error("points has too many/few elements");
    }

    char filename1[128];
    char filename2[128];

    if (__final) {
        sprintf(filename1, "res1.txt");
        sprintf(filename2, "res2.txt");
    } else {
        sprintf(filename1, "res1_%d.txt", iteration);
        sprintf(filename2, "res2_%d.txt", iteration);
        iteration++;
    }

    std::ofstream f1(filename1);
    std::ofstream f2(filename2);

    f1 << std::fixed << std::setprecision(1);
    f2 << std::fixed << std::setprecision(1);

    for (int i = 0; i < first_len; i++) {
        f1 << points[i].x << " " << points[i].y << "\n";
    }
    for (int i = first_len; i < (first_len + second_len); i++) {
        f2 << points[i].x << " " << points[i].y << "\n";
    }
}