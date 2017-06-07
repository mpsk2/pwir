#ifndef COLLISIONS_FILEREADER_HH
#define COLLISIONS_FILEREADER_HH

#include <fstream>
#include <vector>

class FileReader : public FileHeader {
private:
    std::string filename;
    std::ifstream file_stream;
public:
    FileReader(const std::string& filename, const int& number) : FileHeader(number), filename(filename), file_stream(filename) {
        this->read_header();
    }

    void read_header();

    Point next();
    std::vector<Point> read_file();
};

#endif //COLLISIONS_FILEREADER_HH
