#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include "util.h"
#include "config.h"

void baca_direktori(int socket, char *direktori, char *direktori_virtual);
