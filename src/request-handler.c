#include "http-codes.h"
#include "request-handler.h"
#include "response.h"

char **prepare_environment(char *method, char *pathasli, char *path_virtual, char *query_string) {
    char **env;
	int index;

    env = (char **)malloc(sizeof(char *) * 10);

	index = -1;
	
	index++;
	env[index] = (char *)malloc(sizeof(char) * 20);
    snprintf(env[index], 20, "REQUEST_METHOD=%s", method);

	index++;
    env[index] = (char *)malloc(sizeof(char) * (MAXLEN_QUERYSTRING + 14));
    snprintf(env[index], MAXLEN_QUERYSTRING + 14, "QUERY_STRING=%s", query_string);

	index++;
    env[index] = (char *)malloc(sizeof(char) * 50);
    snprintf(env[index], 50, "REDIRECT_STATUS=1");

	index++;
    env[index] = (char *)malloc(sizeof(char) * (MAXLEN_PATH + 15));
    snprintf(env[index], MAXLEN_PATH + 15, "DOCUMENT_ROOT=%s", document_root);
	
	index++;
    env[index] = (char *)malloc(sizeof(char) * (MAXLEN_PATH + 17));
    snprintf(env[index], MAXLEN_PATH + 17, "SCRIPT_FILENAME=%s", pathasli);

	index++;
    env[index] = (char *)malloc(sizeof(char) * (MAXLEN_PATH + 17));
    snprintf(env[index], MAXLEN_PATH + 17, "PATH_TRANSLATED=%s", pathasli);

	index++;
    env[index] = (char *)malloc(sizeof(char) * (MAXLEN_PATH + 13));
    snprintf(env[index], MAXLEN_PATH + 13, "REQUEST_URI=%s", path_virtual);

	index++;
    env[index] = (char *)malloc(sizeof(char) * (MAXLEN_PATH + 13));
    snprintf(env[index], MAXLEN_PATH + 13, "SCRIPT_NAME=%s", path_virtual);
	
	index++;
    env[index] = (char *)malloc(sizeof(char) * (strlen(SERVER_NAME) + 17));
    snprintf(env[index], MAXLEN_PATH + 13, "SERVER_SOFTWARE=%s", SERVER_NAME);

	index++;
	env[index] = (char *)0;
	
	return env;	
}

void process_action(int socket, char *method, char *handler, char *pathasli, char *path_virtual, char *query_string) {

    char **param;
    char **env;
	
	if (cek_tipe_file(handler) == 0) {
		send_error(socket, HTTP_INTERNAL_SERVER_ERROR, NULL);
		return;
	}

    param = (char **)malloc(sizeof(char *) * 1);

    param[0] = (char *)malloc(sizeof(char) * MAXLEN_PATH);
    strcpy(param[0], pathasli);
	
	env = prepare_environment(method, pathasli, path_virtual, query_string);
	
    execute_cgi(socket, handler, param, env);
}

void proses_script(int socket, char *method, char *pathasli, char *path, char *query_string) {
	
	char **param;
	char **env;
	
	if (cek_tipe_file(pathasli) != TIPE_FILE) {
		send_error(socket, HTTP_NOT_FOUND, path);
		return;
	}

    param = (char **)malloc(sizeof(char *) * 1);

    param[0] = (char *)malloc(sizeof(char) * MAXLEN_PATH);
    strcpy(param[0], pathasli);

	env = prepare_environment(method, pathasli, path, query_string);
	
	send_str(socket, "HTTP/1.0 200 OK\r\n");
	execute_cgi(socket, pathasli, param, env);
}

void proses_file(int socket, char *pathasli, char *path_virtual, char *query_string) {
	char *c;
	char *handler;
	char *content_type;
	
	c = strrchr(pathasli, '.');
	content_type = get_content_type(c);
	if ((handler = check_handler(content_type)) != NULL) {
#ifdef DEBUG
		printf("CGI\n"); fflush(stdout);
		printf("handler[%s]\n", handler);
#endif
		process_action(socket, "GET", handler, pathasli, path_virtual, query_string);
	}
	else {
#ifdef DEBUG
		printf("REGULAR FILE\n"); fflush(stdout);
#endif
		kirim_file(socket, pathasli, content_type);
	}
	
}

void cek_direktori(int socket, char *pathasli, char *path, char *query_string) {

	char cek_file[MAXLEN_PATH];
	int i;
	int cek_tipe;
	int ada = 0;
	char *c;
	
	if (pathasli[strlen(pathasli)-1] != '/') {
		strcat(path, "/");
		send_redirect(socket, HTTP_MOVE_PERMANENTLY, path);
		return;
	}

	for (i=0; i<directory_index_total; i++) {
		strcpy(cek_file, pathasli);
		strcat(cek_file, directory_index[i]);

		cek_tipe = cek_tipe_file(cek_file);
		if (cek_tipe == TIPE_FILE) {
			ada = 1;
			break;
		}
	}
	
	if (!ada) {
#ifdef DEBUG
		printf("baca_direktori: pathasli[%s] path[%s]", pathasli, path);
#endif
		send_successful(socket, HTTP_OK);
		send_content_type(socket, "text/html");
		send_server_name(socket);
		send_str(socket, "\r\n");		
		baca_direktori(socket, pathasli, path);
	}
	else {
#ifdef DEBUG
		printf("kirim_file: cek_file[%s] pathasli[%s] path[%s]", cek_file, pathasli, path);
#endif
		proses_file(socket, cek_file, path, query_string);
	}
}

void tangani_request_get(int socket, char *path) {
	
	int tipe;
	char pathasli[MAXLEN_PATH];
	char query_string[MAXLEN_QUERYSTRING];
    char *content_type;
    char *handler;
    char *c;
	struct t_script_alias *script_alias;
	
	//cek script alias
	script_alias = cek_script_alias(path);
	if (script_alias != NULL) {
		
		c = path;
		c += strlen(script_alias->script_alias);
		bersihkan_path(c, query_string);
		
		pathasli[0] = '\0';
		if (script_alias->real_path[0] != '/') {
			strcat(pathasli, server_root);
		}
		strcat(pathasli, script_alias->real_path);
		strcat(pathasli, c);
		
		proses_script(socket, "GET", pathasli, path, query_string);
	}
	else {
		strcpy(pathasli, document_root);
		
		memset(query_string, '\0', sizeof(char) * MAXLEN_QUERYSTRING);
		
		bersihkan_path(path, query_string);
		
		strcat(pathasli, path);
		
		tipe = cek_tipe_file(pathasli);

#ifdef DEBUG
		printf("path[%s]\npathasli[%s]\ntipe[%d]\n", path, pathasli, tipe);
#endif
	
		if (tipe == TIPE_FILE) {
			proses_file(socket, pathasli, path, query_string);
		}
		else if (tipe == TIPE_DIREKTORI) {
			cek_direktori(socket, pathasli, path, query_string);
		}
		else {
			send_error(socket, HTTP_NOT_FOUND, path);
		}
	}
}

/**
 * Menangani koneksi yang masuk.
 *
 * @param  csd  socket descriptor milik client
 */
void tangani_request(int csd) {
    char buffer[BUFFER_SIZE];
    
    char method[10];
    char path[256];
    char version[15];
	
    read_line(csd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%s %s %[^\r]", method, path, version);

#ifdef DEBUG
    printf("method:[%s] path:[%s] version:[%s]\n", method, path, version);
#endif

	//buang header yang lain
    while (read_line(csd, buffer, BUFFER_SIZE) > 0) {
#ifdef DEBUG
        printf("header [%s]\n", buffer);
#endif
    }
	
	if (strcmp(method, "GET") == 0) {
		tangani_request_get(csd, path);
	}
	else {
		send_error(csd, HTTP_NOT_IMPLEMENTED, NULL);
	}
}

int send_content_type(int socket, char *content_type) {
	
	send_str(socket, "Content-type: ");
	send_str(socket, content_type);
	send_str(socket, "\r\n");	
	
}

int send_server_name(int socket) {
	send_str(socket, "Server: ");
	send_str(socket, SERVER_NAME);
	send_str(socket, "\r\n");
}

int kirim_file(int socket, char *namaFile, char *content_type) {
	FILE *f;
	char *buffer;
	int stop = 0;
	int banyak = 0;
	
#ifdef DEBUG
	printf("fname[%s] ct[%s]\n", namaFile, content_type);
#endif

    f = fopen(namaFile, "r");
    buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);

	send_successful(socket, HTTP_OK);
	send_content_type(socket, content_type);
	send_server_name(socket);
	send_str(socket, "\r\n");
	
	while (!stop) {
		banyak = fread(buffer, sizeof(char), 150, f);

		if (banyak == 0) {
			stop = 1;
		}
		else {
			send(socket, buffer, banyak, 0);
		}
	}
}
