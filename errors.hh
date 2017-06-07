#ifndef COLLISIONS_ERRORS_HH
#define COLLISIONS_ERRORS_HH

#include <string>

void handle_error();
void handle_error(std::string msg);

#define PRINTF_FL(format, ...) fprintf(stderr, "[%s:%s:%d]" format "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#endif //COLLISIONS_ERRORS_HH
