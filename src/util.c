#include "util.h"
#include "config.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * Mengambil 1 baris data yang dikirimkan melalui jaringan.
 *
 * @param   id        socket descriptor
 * @param   *buffer   tempat menampung karakter-karakter yang dikirim
 * @param   maxlen    panjang maksimal yang dikehendaki
 * @return  banyak karakter yang diterima
 */
int read_line(int sd, char *buffer, int maxlen) {
    int i;
    char c;
    int ttl;
    int res;
    char *pos = buffer;

    res = 0;
    
    for (i=1; i<maxlen; i++) {
        ttl = recv(sd, &c, 1, 0);
        if (ttl > 0) {
            if (c != '\r') {
                *pos = c;
                pos++;
                res++;
            }
            else {
                ttl = recv(sd, &c, 1, MSG_PEEK);
                if (c == '\n') {
                    recv(sd, &c, 1, 0);
                }
                
                *pos = '\0';
                break;
            }
        }
        else {
            if (ttl == 0) {
                ttl = -1;
            }
            break;
        }
    }
    *pos = '\0';
    
    return res;
}

/**
 * Konversi bilangan heksadesimal menjadi desimal.
 * 
 * @param   *chr  pointer ke dua karakter berurutan yang berisi nilai 
 *                heksadesimal
 * @return  bilangan desimal
 */
int hex2dec(char *chr) {
    int res = 0;
    int i;
    
    for (i=0; i<2; i++) {
        if (upcase(chr[i]) >= 'A') { res += (upcase(chr[i]) - 'A' + 10) << (4 * (1-i)); }
        else { res += (chr[i] - '0') << (4 * (1-i)); }
    }

    return res;
}

void konversi_escaped_character(char *str) {
	int max, len, i, j;
	
	len = strlen(str);
	
    j = 0;
    max = len;
    for (i=0; i<max; i++) {
        if (str[i] == '%') {
            str[j] = hex2dec(&str[i+1]);
            i += 2;
            len -= 2;
        }
        else {
            str[j] = str[i];
        }
        j++;
    }
    str[j] = '\0';
}

/**
 * Membersihkan path pada URL dan mengkonversi %xx menjadi karakter ascii-nya.
 */
void bersihkan_path(char *path, char *query_string) {
    
    int i, j, len, max;
	
	char *c;
	c = strchr(path, '?');
	if (c != NULL) {
		*c = '\0';
		c++;
		strcpy(query_string, c);
		konversi_escaped_character(query_string);
	}
	else {
		query_string[0] = '\0';
	}

    len = strlen(path);

#ifdef DEBUG
    printf("awal:[%d][%s]\n", len, path);
#endif

    //ganti /./ menjadi / dan // menjadi /
    j = 0;
    max = len;
    for (i=0; i<max; i++) {
        if ((path[i] == '/') && (path[i+1] == '.') && (path[i+2] == '/')) {
            i++;
            len -= 2;
        }
        else if ((path[i] == '/') && (path[i+1] == '/')) {
            len--;
        }
        else {
            path[j] = path[i];
            j++;
        }
    }
    path[j] = '\0';

#ifdef DEBUG
    printf("/.//:[%d][%s]\n", len, path);
#endif
    
    //proses escaped character
	konversi_escaped_character(path);
    
#ifdef DEBUG
    printf("%%xx:[%d][%s]\n", len, path);
#endif
    
    //ganti ./ menjadi /
    if (len > 2) {
        if ((path[0] == '.') && (path[1] == '/')) {
            //geser!
            for (i=0; i<len-1; i++) {
                path[i] = path[i+1];
            }
            path[i+1] = '\0';
            len--;
        }
    }
    
#ifdef DEBUG
    printf(" ./ :[%d][%s]\n", len, path);
#endif
    
    //ganti /. menjadi /
    if (len > 2) {
        if ((path[len-2] == '/') && (path[len-1] == '.')) {
            path[len-1] = '\0';
            len--;
        }
    }

#ifdef DEBUG
    printf(" /. :[%d][%s]\n", len, path);
#endif

}

/**
 * Mengirim string melalui soket.
 *
 * @param  socket  socket descriptor
 * @param  *str    string yang akan dikirim
 * @return status sesuai fungsi send()
 */
int send_str(int socket, const char *str) {
#ifdef DEBUG
	printf("send(socket[%d], str[%s], strlen[%d], 0)", socket, str, strlen(str));
#endif
	
	return send(socket, str, strlen(str), 0);
}

/** 
 * Mengambil content-type dari suatu ekstensi file
 *
 * @param  *ekstensi       ekstensi file (titik diikutsertakan)
 * @param  *content_type   content-type
 * @return pointer ke content-type
 */
char *get_content_type(char *ekstensi) {
	struct t_content_type *ct;
	struct t_extension *ext;
	int ada;
	char *content_type;
		
	ct = daftar_content_type;
	ext = ct->daftar_ekstensi;
	ada = 0;
	
	content_type = default_content_type;
	while (ct != NULL) {

		ext = ct->daftar_ekstensi;
		while (ext != NULL) {
			if (strcmp(ext->ekstensi, ekstensi) == 0) {
				content_type = ct->content_type;
				ada = 1;
				break;
			}
			
			ext = ext->next;
		}
		
		if (ada) {
			break;
		}
		ct = ct->next;
	}
	
	return content_type;
}

/**
 * Mengecek tipe file
 * 
 * @param   nama file
 * @return  tipe file, 0 jika tidak ada
 */
int cek_tipe_file(char *path) {

	int hasil;
	struct stat statku;

	int statnya = stat(path, &statku);

	if (S_ISDIR(statku.st_mode)) {
		hasil = TIPE_DIREKTORI;
	}
	else if (S_ISREG(statku.st_mode)) {
		hasil = TIPE_FILE;
	}
	else {
		hasil = 0;
	}

	return hasil;
}

/**
 * Mengecek apakah ada handler untuk suatu tipe file tertentu.
 */
char *check_handler(char *content_type) {
    struct t_content_type *pct;
    int ada = 0;
	char *handler;

    handler = NULL;
    
    pct = daftar_content_type;
    while (pct != NULL) {
        if (strcmp(content_type, pct->content_type) == 0) {
            if (pct->handler != NULL) {
                handler = pct->handler;
                ada = 1;
            }
            break;
        }
        pct = pct->next;
    }
	
    return handler;
}

/**
 * Mengecek apakah suatu path ada dalam script alias.
 */
struct t_script_alias *cek_script_alias(char *path) {
	struct t_script_alias *psa;
	struct t_script_alias *res;
		
#ifdef DEBUG
	printf("cek_script_alias(path[%s])\n", path);
#endif
		
	res = NULL;
	
	psa = daftar_script_alias;
	while (psa != NULL) {
		if (strncmp(path, psa->script_alias, strlen(psa->script_alias)) == 0) {
			res = psa;
			break;
		}
		psa = psa->next;
	}
	
	return res;
}

/**
 * Buat server signature.
 */
void create_server_signature() {
	
	snprintf(server_signature, MAXLEN_SIGNATURE, "%s at %s Port %d", SERVER_NAME, SERVER_HOST, server_port);
	
}

/**
 * Konversi menjadi huruf kapital.
 */
int upcase(int chr) {
	if ((chr >= 'a') && (chr <= 'z')) {
		chr = chr - 'a' + 'A';
	}
	return chr;
}
