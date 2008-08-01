#include "config.h"

#define BUFFER_SIZE 256

struct t_konfigurasi {
	char nama[50];
	char nilai[200];
};

/**
 * Tambahkan content-type ke dalam daftar
 */
void tambah_content_type(char *ct, char *ext) {
	struct t_content_type *pct, *lpct;
	struct t_extension *pext, *lpext;
	int ada;

#ifdef DEBUG
	printf("tambah_content_type(\"%s\", \"%s\")\n", ct, ext);	
#endif
	
	if (daftar_content_type == NULL) {
		daftar_content_type = (struct t_content_type *)malloc(sizeof(struct t_content_type));
		strcpy(daftar_content_type->content_type, ct);
		daftar_content_type->next = NULL;
		daftar_content_type->daftar_ekstensi = (struct t_extension *)malloc(sizeof(struct t_extension));
		strcpy(daftar_content_type->daftar_ekstensi->ekstensi, ext);
		daftar_content_type->daftar_ekstensi->next = NULL;
	}
	else {
		pct = daftar_content_type;
		
		ada = 0;
		while (pct != NULL) {
			if (strcmp(pct->content_type, ct) == 0) {
				
				pext = pct->daftar_ekstensi;
				while (pext != NULL) {
					if (strcmp(pext->ekstensi, ext) == 0) {
						ada = 1;
						break;
					}
					
					lpext = pext;
					pext = pext->next;
				}
				
				//ada content-type, belum ada ekstensi
				if (!ada) {
					lpext->next = (struct t_extension *)malloc(sizeof(struct t_extension));
					pext = lpext->next;
					
					pext->next = NULL;
					strcpy(pext->ekstensi, ext);
					ada = 1;
				}
				break;
			}
			
			lpct = pct;
			pct = pct->next;
		}
		
		//belum ada content-type
		if (!ada) {
			lpct->next = (struct t_content_type *)malloc(sizeof(struct t_content_type));
			pct = lpct->next;
			
			pct->next = NULL;
            pct->handler = NULL;
			strcpy(pct->content_type, ct);
			
			pct->daftar_ekstensi = (struct t_extension *)malloc(sizeof(struct t_extension));
			pext = pct->daftar_ekstensi;
			
			pext->next = NULL;
			strcpy(pext->ekstensi, ext);
			
		}
	
	}		
}

/**
 * Atur handler suatu tipe file tertentu.
 */
void set_handler(char *content_type, char *handler) {
    struct t_content_type *pct;

#ifdef DEBUG
    printf("set_handler(ct[%s], handler[%s])\n", content_type, handler);
    fflush(stdout);
#endif

    pct = daftar_content_type;
    while (pct != NULL) {
        if (strcmp(pct->content_type, content_type) == 0) {
            pct->handler = (char *)malloc(sizeof(char) * (strlen(handler) + 1));
            strcpy(pct->handler, handler);
            break;
        }
        pct = pct->next;
    }

}

/** 
 * Tambahkan script alias.
 */
void tambah_script_alias(char *script_alias, char *real_path) {
	struct t_script_alias *psa, *lpsa;
	int ada;
	
	if (daftar_script_alias == NULL) {
		daftar_script_alias = (struct t_script_alias *)malloc(sizeof(struct t_script_alias));
		strcpy(daftar_script_alias->script_alias, script_alias);
		strcpy(daftar_script_alias->real_path, real_path);
		daftar_script_alias->next = NULL;
	}
	else {
		psa = daftar_script_alias;
		
		ada = 0;
		while (psa != NULL) {
			if (strcmp(psa->script_alias, script_alias) == 0) {
				strcpy(psa->real_path, real_path);
				ada = 1;
				break;
			}
			lpsa = psa;
			psa = psa->next;
		}
		if (!ada) {
			lpsa->next = (struct t_script_alias *)malloc(sizeof(struct t_script_alias));
			psa = lpsa->next;
			
			strcpy(psa->script_alias, script_alias);
			strcpy(psa->real_path, real_path);
			psa->next = NULL;
		}
	}
}

/**
 * set nilai default
 */
void set_default() {
	
	//ServerPort
	server_port = 8000;
	
	//DocumentRoot
	strcpy(document_root, "htdocs/");
	
	//DirectoryIndex
	directory_index_total = 1;
	directory_index[0] = (char *)malloc(sizeof(char) * 11);
	strcpy(directory_index[0], "index.html");
	
	//Content-Type
	daftar_content_type = NULL;
    strcpy(default_content_type, "text/plain");
	
	//Script Alias
	daftar_script_alias = NULL;
	
	//ServerRoot
	server_root = (char *)malloc(sizeof(char) * MAXLEN_PATH);
	getcwd(server_root, MAXLEN_PATH);
	if (server_root[strlen(server_root)-1] != '/') {
		strcat(server_root, "/");
	}
}

/**
 * Pisahkan field konfigurasi dan nilainya
 */
void pisah_nilai(char *baris, struct t_konfigurasi *k) {
	char *c;
	
	if (strchr(baris, ' ') == NULL) {
		return;
	}
	
	//potong sampai spasi
	c = strchr(baris, ' ');
	if (c != NULL) {
		*c = '\0';
	}
	
	//nama
	strcpy(k->nama, baris);

	//copy sisa baris
	c++;
	strcpy(k->nilai, c);
	
	//potong sampai \n
	c = strchr(k->nilai, '\n');
	if (c != NULL) {
		*c = '\0';
	}
	
	//potong sampai \r
	c = strchr(k->nilai, '\r');
	if (c != NULL) {
		*c = '\0';
	}
}

/**
 * baca file konfigurasi.
 */
void baca_konfigurasi() {
	FILE *f;
	char buffer[BUFFER_SIZE];
	struct t_konfigurasi konfigurasi;
	char *c, *a;
	char *ct, *ext;
	int i, j, k;

	//set nilai default
	set_default();

	//buka file konfigurasi
	f = fopen(CONFIG_FILE, "r");
	while (!feof(f)) {
		
		if (fgets(buffer, BUFFER_SIZE, f) != NULL) {
			
#ifdef DEBUG
			printf("konfigurasi[%s]\n", buffer);
#endif

			if ((strlen(buffer) <= 1) || (buffer[0] == '#')) {
				continue;					
			}
			
			pisah_nilai(buffer, &konfigurasi);
	
			//ServerPort
			if (strcmp(konfigurasi.nama, "ServerPort") == 0) {
				server_port = atoi(konfigurasi.nilai);
			}
			//DocumentRoot
			else if (strcmp(konfigurasi.nama, "DocumentRoot") == 0) {
				strcpy(document_root, konfigurasi.nilai);
				if (document_root[strlen(document_root)-1] == '/') {
					document_root[strlen(document_root)-1] = '\0';
				}
			}
			//DirectoryIndex
			else if (strcmp(konfigurasi.nama, "DirectoryIndex") == 0) {
				i = 0;
				a = konfigurasi.nilai;
				while ((c = strchr(a, ' ')) != NULL) {
					
					j = c - a + 1;
					directory_index[i] = (char *)malloc(j);
					strncpy(directory_index[i], a, j - 1);
					
					a += j;
					
					i++;
					if (i == 9) {
						break;
					}
				}
				if (strlen(a) > 0) {
					directory_index[i] = (char *)malloc(strlen(a));
					strcpy(directory_index[i], a);
				}
				directory_index_total = i + 1;
			}
			//AddType
			else if (strcmp(konfigurasi.nama, "AddType") == 0) {
				c = strchr(konfigurasi.nilai, ' ');
				*c = '\0';
				c++;
				tambah_content_type(konfigurasi.nilai, c);
			}
            //DefaultType
            else if (strcmp(konfigurasi.nama, "DefaultType") == 0) {
                strcpy(default_content_type, konfigurasi.nilai);
            }
            //Action
            else if (strcmp(konfigurasi.nama, "Action") == 0) {
				fflush(stdout);
                c = strchr(konfigurasi.nilai, ' ');
                *c = '\0';
                c++;
                set_handler(konfigurasi.nilai, c);
            }
			//ScriptAlias
			else if (strcmp(konfigurasi.nama, "ScriptAlias") == 0) {
				c = strchr(konfigurasi.nilai, ' ');
				*c = '\0';
				c++;
				tambah_script_alias(konfigurasi.nilai, c);
			}
			//ServerRoot
			else if (strcmp(konfigurasi.nama, "ServerRoot") == 0) {
				strcpy(server_root, konfigurasi.nilai);
				if (server_root[strlen(server_root)-1] != '/') {
					strcat(server_root, "/");
				}
			}
		}			
	}
	
	//pindah ke DocumentRoot
	if (document_root[0] != '/') {
		strcpy(buffer, server_root);
		strcat(buffer, document_root);
		strcpy(document_root, buffer);
	}
	chdir(document_root);
	getcwd(document_root, MAXLEN_PATH);
	
	fclose(f);
}
