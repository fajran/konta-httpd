#include <stdio.h>

#define SERVER_NAME "Konta-HTTP Server"
#define SERVER_HOST "localhost"
#define CONFIG_FILE "conf/konta-httpd.conf"
#define MAX_QUEUE 5
#define BUFFER_SIZE 256
#define MAX_DIRECTORY_INDEX 10
#define MAXLEN_PATH 256
#define MAXLEN_CONTENT_TYPE 25
#define MAXLEN_EXTENSION 10
#define MAXLEN_QUERYSTRING 256
#define MAXLEN_SIGNATURE 256

void baca_konfigurasi();

#ifndef STRUCT_T_EXTENSION
struct t_extension {
	char ekstensi[MAXLEN_EXTENSION];
	struct t_extension *next;
};
#define STRUCT_T_EXTENSION
#endif

#ifndef STRUCT_T_CONTENT_TYPE
struct t_content_type {
	char content_type[MAXLEN_CONTENT_TYPE];
	struct t_extension *daftar_ekstensi;
	struct t_content_type *next;
    char *handler;
};
#define STRUCT_T_CONTENT_TYPE
#endif

#ifndef STRUCT_T_SCRIPT_ALIAS
struct t_script_alias {
	char script_alias[MAXLEN_PATH];
	char real_path[MAXLEN_PATH];
	struct t_script_alias *next;
};
#define STRUCT_T_SCRIPT_ALIAS
#endif

/* konfigurasi */
int server_port;
char document_root[MAXLEN_PATH];
char *directory_index[MAX_DIRECTORY_INDEX];
int directory_index_total;
struct t_content_type *daftar_content_type;
char default_content_type[MAXLEN_CONTENT_TYPE];
struct t_script_alias *daftar_script_alias;
char *server_root;

/* global */
char server_signature[MAXLEN_SIGNATURE];
