#ifndef COLLISIONS_SENDER_HH
#define COLLISIONS_SENDER_HH

#include <vector>
#include "Point.hh"

class Sender {
private:
    const int process_number;
    const int gal_1_stars;
    const int gal_2_stars;
public:
    Sender(const int& __process_number, const int& __gal_1_stars, const int& __gal_2_stars) noexcept :
            process_number(__process_number),
            gal_1_stars(__gal_1_stars),
            gal_2_stars(__gal_2_stars) {
    }

    std::vector<Point> sent_initial(const std::vector<Point>&);
};

#endif //COLLISIONS_SENDER_HH
