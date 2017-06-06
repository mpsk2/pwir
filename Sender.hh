#ifndef COLLISIONS_SENDER_HH
#define COLLISIONS_SENDER_HH

#include <vector>
#include "Point.hh"

class Sender {
private:
    const int process_number;
    const int processes_count;
    const int gal_1_stars;
    const int gal_2_stars;
    const bool verbose;
    const int hor;
    const int ver;

    int should_receive_hor;
    int should_receive_ver;
public:
    Sender(const int& __process_number, const int& __processes_count, const int& __gal_1_stars,
           const int& __gal_2_stars, const bool& __verbose, const int& __hor, const int& __ver) noexcept :
            process_number(__process_number),
            processes_count(__processes_count),
            gal_1_stars(__gal_1_stars),
            gal_2_stars(__gal_2_stars),
            verbose(__verbose),
            ver(__ver),
            hor(__hor) {}

    std::vector<Point> sent_initial(const std::vector<Point>& __data);
    std::vector<Point> sent_initial_all(const std::vector<Point>& __data);
    std::vector<Point> distribute_chunks(const chunks_t& chunks);
    void send_neighbour(const std::vector<Point> &__my_chunk, const int& x, const int& y, const int& tag);
    std::vector<std::vector<Point> > distribute_neighbours(const std::vector<Point>& __my_chunk);
    std::vector<Point> receive_neighbour(const int& x, const int& y, const int& tag);
    std::vector<std::pair<int, int> > neighbours();

    std::vector<Point> redistribute(std::vector<Point>& __data);
};

#endif //COLLISIONS_SENDER_HH
