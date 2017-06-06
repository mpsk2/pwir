#include <iostream>
#include <mpi.h>
#include <vector>
#include <unistd.h>
#include <sstream>
#include "FileHeader.hh"
#include "FileReader.hh"
#include "Point.hh"
#include "PMISerializable.hh"
#include "cli.hh"
#include "errors.hh"
#include "Sender.hh"
#include "bounds.hh"
#include "Algorithm.hh"
#include "Verbose.hh"

int main(int argc, char** argv) {
    auto arguments = Arguments::from_cli(argc, argv);

    // Check whenever input files exists
    if (access(arguments.galaxy_1_file, F_OK) == -1) {
        std::cerr << "File " << arguments.galaxy_1_file << " does not exists.\n";
        exit(1);
    }

    if (access(arguments.galaxy_2_file, F_OK) == -1) {
        std::cerr << "File " << arguments.galaxy_2_file << " does not exists.\n";
        exit(1);
    }

    int process_number;
    int processes_count;
    std::vector<Point> points;
    FileHeader *fr1;
    FileHeader *fr2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    MPI_Comm_size(MPI_COMM_WORLD, &processes_count);

    if (processes_count < 2) {
        handle_error("Too few processes.");
    }
    if (processes_count != arguments.horizontal_cells * arguments.vertical_cells) {
        std::stringstream ss;
        ss << "There should be ";
        ss << arguments.horizontal_cells;
        ss << "x";
        ss << arguments.vertical_cells;
        ss << " processes, but there are ";
        ss << processes_count;
        ss << " processes.";
        handle_error(ss.str());
    }

    FileHeader::create_type();
    Point::create_type();

    const int part_x = process_number % arguments.horizontal_cells;
    const int part_y = process_number / arguments.horizontal_cells;

    if(process_number == 0) {
        fr1 = new FileReader(arguments.galaxy_1_file, 1);
        fr2 = new FileReader(arguments.galaxy_2_file, 2);

        points = dynamic_cast<FileReader*>(fr1)->read_file();
        std::vector<Point> coords2 = dynamic_cast<FileReader*>(fr2)->read_file();
        points.insert(points.end(), coords2.begin(), coords2.end());

        auto space = simulation_space(points);

        fr1->bound_left = fr2->bound_left = std::get<0>(space);
        fr1->bound_right = fr2->bound_right = std::get<1>(space);
        fr1->bound_down = fr2->bound_down = std::get<2>(space);
        fr1->bound_up = fr2->bound_up = std::get<3>(space);

        fr1->broadcast();
        fr2->broadcast();
    } else {
        fr1 = &FileHeader::receive();
        fr2 = &FileHeader::receive();
        if (fr1->number == 2) {
            std::swap(fr1, fr2);
        }
    }

    Sender sender(process_number, processes_count, fr1->stars_number, fr2->stars_number, arguments.verbose,
                  arguments.horizontal_cells, arguments.vertical_cells);

    std::vector<Point> data = sender.sent_initial(points);

    auto mb = my_bounds(std::make_tuple(fr1->bound_left, fr1->bound_right, fr1->bound_down, fr1->bound_up), part_x,
                        part_y, arguments.horizontal_cells, arguments.vertical_cells);

    auto amb = area_bounds(std::make_tuple(fr1->bound_left, fr1->bound_right, fr1->bound_down, fr1->bound_up), part_x,
                           part_y, arguments.horizontal_cells, arguments.vertical_cells);

    if ((process_number == 0) && arguments.verbose) {
        write_file(data, fr1->stars_number, fr2->stars_number, false);
    }

    // Point::fill_accelerations(data);

    for (int i = 0; i < arguments.total / arguments.delta; i++) {
        auto sub_data = my_chunk(data, mb);

        sub_data = step_chunk(sub_data, data, arguments.delta);
        sub_data = borders(sub_data, fr1->borders());
        data = sender.redistribute(sub_data);

        if ((process_number == 0) && arguments.verbose) {
            for (const auto &d : data) {
                PRINTF_FL("P=%s", d.str().c_str());
            }
            write_file(data, fr1->stars_number, fr2->stars_number, false);
        }
    }

    if (process_number == 0) {
        write_file(data, fr1->stars_number, fr2->stars_number, true);
    }


    MPI_Finalize();
    return 0;
}