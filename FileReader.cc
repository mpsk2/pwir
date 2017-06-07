#include "PMISerializable.hh"
#include "Point.hh"
#include "bounds.hh"
#include "FileHeader.hh"
#include "FileReader.hh"

void FileReader::read_header() {
    this->file_stream >> this->stars_number;
    this->file_stream >> this->speed_x >> this->speed_y;
    this->file_stream >> this->mass;
}

Point FileReader::next() {
    static int n=0;
    Point::coord_t x, y;
    this->file_stream >> x >> y;
    return Point(x, y, this->speed_x, this->speed_y, this->mass, n++);
}

std::vector<Point> FileReader::read_file() {
    std::vector<Point> result;
    for (int i = 0; i < this->stars_number; i++) {
        result.push_back(this->next());
    }
    return result;
}