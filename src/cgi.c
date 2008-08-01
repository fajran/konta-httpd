#include "cgi.h"

#define FD_STDOUT 1

void execute_cgi(int socket, char *filename, char *param[], char *env[]) {
    int pid;
    int fd[2];
    char buffer[BUFFER_SIZE];
    int n;
	int cnt;
	char status[50];

    pipe(fd);

    pid = fork();
    if (pid == 0) { /* child */

#ifdef DEBUG
        printf("execve([%s], [%s], [%s] [%s] [%s])\n", filename, param[0], env[0], env[1], env[2]);
#endif

        close(fd[0]);
        dup2(fd[1], FD_STDOUT);
        close(fd[1]);

		execle(filename, param[0], param[0], (char *)0, env);
            
        exit(0);
    }
    else if (pid > 0) {

        close(fd[1]);

		memset(status, '\0', sizeof(status));
		
		//check status
		n = read(fd[0], buffer, BUFFER_SIZE);
		strncpy(status, buffer, 7);

		if (strcmp(status, "Status:") != 0) {
			send_str(socket, "HTTP/1.0 200 OK\r\n");
		}
		else {
			status[0] = buffer[8];
			status[1] = buffer[9];
			status[2] = buffer[10];
			status[3] = '\0';
			send_str(socket, "HTTP/1.0 ");
			send_str(socket, status);
			send_str(socket, "\r\n");
		}
		send(socket, buffer, n, 0);		
        while ((n = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
			cnt = 1;
            send(socket, buffer, n, 0);
        }
         
        close(fd[0]);

    }
	else {
		close(fd[0]);
		close(fd[1]);
		send_error(socket, HTTP_INTERNAL_SERVER_ERROR, NULL);
	}
}
