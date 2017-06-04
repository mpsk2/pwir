#ifndef COLLISIONS_CLI_HH
#define COLLISIONS_CLI_HH

#include <string>

class Arguments {
public:
    bool verbose;
    int horizontal_cells;
    int vertical_cells;
    double delta;
    double total;
    char *galaxy_1_file;
    char *galaxy_2_file;

    std::string str() const;
    static Arguments from_cli(const int& argc, char** argv);

};

#endif //COLLISIONS_CLI_HH
