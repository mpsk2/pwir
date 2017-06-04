#include <sstream>
#include <argp.h>
#include "cli.hh"

const char *argp_program_version =
        "0.1";
const char *argp_program_bug_address =
        "ms335789@students.mimuw.edu.pl";

static char doc[] =
        "MPI project for PWiR MIM UW 2016/2017";

static char args_doc[] = "--hor INT --ver INT --gal1 FILE_PATH --gal2 FILE_PATH --delta FLOAT --total FLOAT";

#define HOR_KEY 1
#define VER_KEY 2
#define GAL1_KEY 3
#define GAL2_KEY 4

static struct argp_option options[] = {
        {"verbose", 'v', 0, 0, "Prodce verbose output"},
        {"hor", HOR_KEY, "-cells", 0, "Horizontal cells"},
        {"ver", VER_KEY, "-cells", 0, "Vertical cells"},
        {"delta", 'd', "-value", 0, "Step time delta"},
        {"total", 't', "-value", 0, "Total simulation time"},
        {"gal1", GAL1_KEY, "-file name", 0, "First galaxy file path"},
        {"gal2", GAL2_KEY, "-file name", 0, "Second galaxy file path"},
        { 0 }
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    Arguments* arguments = (Arguments*) state->input;

    switch (key)
    {
        case HOR_KEY:
            arguments->horizontal_cells = atoi(arg);
            break;
        case VER_KEY:
            arguments->vertical_cells = atoi(arg);
            break;
        case GAL1_KEY:
            arguments->galaxy_1_file = arg;
            break;
        case GAL2_KEY:
            arguments->galaxy_2_file = arg;
            break;
        case 'd':
            arguments->delta = atof(arg);
            break;
        case 't':
            arguments->total = atof(arg);
            break;
        case 'v':
            arguments->verbose = true;
            break;
        case ARGP_KEY_ARG:
            argp_usage(state);
            break;
        case ARGP_KEY_END:
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

Arguments Arguments::from_cli(const int& argc, char** argv) {
    Arguments args;
    argp_parse(&argp, argc, argv, 0, 0, &args);
    return args;
}

std::string Arguments::str() const {
    std::stringstream ss;
    ss << "Printing arguments:\n";
    ss << "Verbose: " << this->verbose << "\n";
    ss << "Horizontal: " << this->horizontal_cells << "\n";
    ss << "Vertical: " << this->vertical_cells << "\n";
    ss << "Delta: " << this->delta << "\n";
    ss << "Total: " << this->total << "\n";
    ss << "Gal 1 file: " << this->galaxy_1_file << "\n";
    ss << "Gal 2 file: " << this->galaxy_2_file << "\n";
    return ss.str();
}