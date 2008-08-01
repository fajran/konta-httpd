#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifdef FORKNEWPROCESS
#include <signal.h>
#endif

#include "util.h"
#include "config.h"
#include "readdir.h"
#include "request-handler.h"

/**
 * Persiapkan soket yang akan digunakan untuk menerima koneksi.
 *
 * @param   port   port yang akan dipakai
 * @return  socket descriptor
 */
int persiapkan_soket(int port) {
    int sd;
    struct sockaddr_in saddr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket()");
        return sd;
    }

    memset(&saddr, '\0', sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        perror("bind()");
		sd = -1;
    }

    if (listen(sd, MAX_QUEUE) < 0) {
        perror("listen()");
		sd = -1;
    }

    return sd;
}

/**
 * Menerima koneksi yang datang.
 *
 * @param   sd  socket yang dipakai
 */
void terima_koneksi(int sd) {
    struct sockaddr_in caddr;
    int caddr_len;

    int csd;
	int pid = 0;

#ifdef FORKNEWPROCESS
	signal(SIGCLD, SIG_IGN);
#endif

    while (1) {
        csd = accept(sd, (struct sockaddr *) &caddr, &caddr_len);

#ifdef FORKNEWPROCESS
		pid = fork();
		if (pid == 0) {
			tangani_request(csd);
			close(sd);
			close(csd);
			exit(0);
		}

#else
        tangani_request(csd);
#endif
        close(csd);

    }

}


int main() {
	
	int server_socket;
	
	baca_konfigurasi();
	create_server_signature();
	
	server_socket = persiapkan_soket(server_port);
	
	if (server_socket > 0) {
		printf("%s running on port %d...\n", SERVER_NAME, server_port);
		fflush(stdout);
		terima_koneksi(server_socket);
	}
	
	close(server_socket);
	
	return 0;
}
