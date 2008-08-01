
#include "http-codes.h"
#include "config.h"

void send_error(int socket, int error_code, char *request_uri);
void send_redirect(int socket, int redirect_code, char *location);
void send_successful(int socket, int success_code);
