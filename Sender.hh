#ifndef COLLISIONS_SENDER_HH
#define COLLISIONS_SENDER_HH

#include <vector>
#include "Point.hh"

class Sender {
public:
    typedef std::vector<std::pair<int, int> > neighbours_t;
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

    const bounds_t bn;
    neighbours_t my_neighbours;

    std::vector<int> self_disp;
    std::vector<int> self_sizes;
public:
    Sender(const int& __process_number, const int& __processes_count, const int& __gal_1_stars,
           const int& __gal_2_stars, const bool& __verbose, const int& __hor, const int& __ver, const bounds_t& __bn) noexcept;

    std::vector<Point> sent_initial(std::vector<Point>& __data);
    std::vector<Point> sent_initial_all(const std::vector<Point>& __data);
    std::vector<Point> sent_initial_neighbours(std::vector<Point>& __data);
    void send_neighbour(const std::vector<Point> &__my_chunk, const int& x, const int& y, const int& tag);
    std::vector<std::vector<Point> > distribute_neighbours(const std::vector<Point>& __my_chunk);
    std::vector<Point> receive_neighbour(const int& x, const int& y, const int& tag);
    std::vector<std::pair<int, int> > neighbours();
    std::vector<std::pair<int, int> > neighbours(const int& __target);

    std::vector<Point> redistribute_all(std::vector<Point>& __data);
    std::vector<Point> redistribute_chunks(std::vector<Point>& __chunks);
    std::vector<Point> redistribute_self(const std::vector<Point>& __chunks);
    std::vector<Point> redistribute(std::vector<Point>& __data);

    std::vector<Point> gather_all_at_root(std::vector<Point>& __data, const int& __sum);

    std::vector<int> gather_sizes(const chunks_t& __data);

    enum OP {
        INITIAL = 1,
        REDISTRIBUTE = 2
    };

    int base_tag(const int& __src, const int& __dest,  const Sender::OP& operation);

    constexpr static int tag_gather_all = 1;
};

#endif //COLLISIONS_SENDER_HH
