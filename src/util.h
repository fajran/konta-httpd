#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int read_line(int sd, char *buffer, int maxlen);
int hex2dec(char *chr);
void bersihkan_path(char *path, char *query_string);
int send_str(int socket, const char *str);
char *get_content_type(char *ekstensi);
int cek_tipe_file(char *path);
char *check_handler(char *content_type);
struct t_script_alias *cek_script_alias(char *path);
void create_server_signature();
int upcase(int chr);

#define TIPE_FILE 2
#define TIPE_DIREKTORI 1
