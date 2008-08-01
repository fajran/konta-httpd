#include "readdir.h"

void fileStatistic(int sock, char *filePath, char *direktori_virtual, DIR* dir_p, struct dirent* dir_entry_p);
char* formatTime(time_t time);
void statisticToSocket(int sock, char* fileName, struct stat *stat_p);
void headerHtml(int sock, char* filePath);
void footerHtml(int sock);

/**
 * Menampilkan isi direktori dalam bentuk HTML.
 *
 * @param  socket      socket descriptor
 * @param  *direktori  lokasi direktori
 */
void baca_direktori(int socket, char *direktori, char *direktori_virtual) {

	DIR* dir_p;	
	struct dirent* dir_entry_p;

	dir_p = opendir(direktori);
	
	headerHtml(socket, direktori_virtual);/*ini buat header htmlnya*/
	fileStatistic(socket, direktori, direktori_virtual, dir_p, dir_entry_p);
	footerHtml(socket);/*ini buat footernya*/
	
	closedir(dir_p);	
	
}

void fileStatistic(int sock, char *filePath, char *direktori_virtual, DIR* dir_p, struct dirent* dir_entry_p) 
{
	int statResult;
	char* fileName = (char *)malloc(sizeof(char) * 200);
	struct stat stat_p;
	char current_dir[MAXLEN_PATH];
	
	getcwd(current_dir, MAXLEN_PATH);
	
	chdir(filePath);
	
	while(NULL != (dir_entry_p = readdir(dir_p)))/*ini buat isi filenya*/
	{
		fileName = (char*)dir_entry_p->d_name;
		
		statResult = stat(fileName, &stat_p);
		if(statResult == -1)
		{
			printf("error when attemping to stat: %s \n", fileName);
			perror("stat");
			//exit(0);
		}
		
		else
		{
			if ((strcmp(direktori_virtual, "/") == 0) && 
				(strcmp(fileName, "..") == 0)) {
				continue;
			}
			statisticToSocket(sock, fileName, &stat_p);
		}
	}
	
}

char* formatTime(time_t time)
{
	static char string[30];
	struct tm* time_struct;

	time_struct = localtime(&time);

	strftime(string, sizeof string, "%h %e %H:%M \n", time_struct);

	return string;
}

void statisticToSocket(int sock, char* fileName, struct stat *stat_p)
{
	char buffer[20];
	
	send_str(sock, "<tr><td>");
	
	send_str(sock, "<a href=\"");
	send_str(sock, fileName);
	send_str(sock, "\">");
	send_str(sock, fileName);
	send_str(sock, "</a>");
	
	send_str(sock, "</td>");
	send_str(sock, "<td align=\"right\">");
	
	snprintf(buffer, 20, "%d", stat_p->st_size);
	send_str(sock, buffer);
	
	send_str(sock, "</td>");
	send_str(sock, "<td>");
	
	send_str(sock, formatTime(stat_p->st_mtime));
	send_str(sock, "</td></tr>");
}

void headerHtml(int sock, char* filePath)
{
	send_str(sock, "<html><head><title>Index of ");
	send_str(sock, filePath);
	send_str(sock, "</title><style type=\"text/css\">td,tr{font-family:monospace;font-size:10pt;}</style></head><body><h1><caption>Index of ");
	send_str(sock, filePath);
	send_str(sock, "</caption></h1>");
	send_str(sock, "<table cellpadding=\"2\"><tr>");
	send_str(sock, "<th>File Name</th>");
	send_str(sock, "<th>File Size</th>");
	send_str(sock, "<th>Last Modified</th>");
	send_str(sock, "</tr>");
}

void footerHtml(int sock)
{
	send_str(sock, "</table>");
	send_str(sock, "<hr/><em>");
	send_str(sock, server_signature);
	send_str(sock, "</em>");
	send_str(sock, "</body></html>");
}
