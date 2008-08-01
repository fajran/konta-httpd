#include "response.h"

void send_error(int socket, int error_code, char *request_uri) {
	
	char buffer[512];

#ifdef DEBUG
	printf("error[%d]\n", error_code);
#endif
	
	if (error_code == HTTP_METHOD_NOT_ALLOWED) {
		send_str(socket, "HTTP/1.0 405 Method Not Allowed\r\n");
		send_str(socket, "Allow: GET\r\n");
		send_str(socket, "\r\n");
	}
	else if (error_code == HTTP_NOT_FOUND) {
		send_str(socket, "HTTP/1.0 404 Not Found\r\n");
		send_str(socket, "Content-type: text/html\r\n");
		send_str(socket, "\r\n");
		send_str(socket, "<html><head><title>404 Not Found</title>");
		send_str(socket, "</head><body><h1>Not Found</h1>");
		send_str(socket, "<p>The requested URL ");
		send_str(socket, request_uri);
		send_str(socket, " was not found on this server.</p><hr/><address>");
		send_str(socket, server_signature);
		send_str(socket, "</address></body></html>");
	}
	else if (error_code == HTTP_NOT_IMPLEMENTED) {
		send_str(socket, "HTTP/1.0 501 Not Implemented\r\n");
		send_str(socket, "\r\n");
	}
	else {
		send_str(socket, "HTTP/1.0 500 Internal Server Error\r\n");
	send_str(socket, "\r\n");
	}
	
}

void send_redirect(int socket, int redirect_code, char *location) {

#ifdef DEBUG
	printf("redirect code[%d] location[%s]\n", redirect_code, location);
#endif

	if (redirect_code == HTTP_MOVE_PERMANENTLY) {
		send_str(socket, "HTTP/1.0 301 Move Permanently\r\n");
		send_str(socket, "Location: ");
		send_str(socket, location);
		send_str(socket, "\r\n");
	}

	send_str(socket, "\r\n");
}

void send_successful(int socket, int success_code) {
	if (success_code == HTTP_OK) {
		send_str(socket, "HTTP/1.0 200 OK\r\n");
	}
}
