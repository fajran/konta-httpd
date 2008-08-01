#include "config.h"
#include "util.h"
#include "http-codes.h"
#include "response.h"

void execute_cgi(int socket, char *filename, char *param[], char *env[]);
