#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define PORT 8080

void resetbuffer(char *s){
	memset(s, 0, sizeof(char) * 1024);
}

void home_ui(char *s){
	resetbuffer(s);
	sprintf(s,"Welcome to database.\n");
	sprintf(s + strlen(s), "Input provided number to continue:\n");
	sprintf(s + strlen(s), "1. Register\n");
	sprintf(s + strlen(s), "2. Login\n");
	sprintf(s + strlen(s), "3. Exit\n");
	sprintf(s + strlen(s), "\n");
}

void goodbye_ui(char *s){
	resetbuffer(s);
	sprintf(s,"Thank you for using our database!\n");
	sprintf(s + strlen(s), "\n");
}

int main(int argc, char const *argv[]) {
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char sendbuffer[1024] = { 0 };

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	bool is_running = true;

	valread = read( new_socket , buffer, 1024);

	while (is_running){
		home_ui(sendbuffer);
		send(new_socket, sendbuffer, 1024 ,0);
		valread = read( new_socket , buffer, 1024);

		resetbuffer(sendbuffer);
		if (strcmp(buffer, "1") == 0){
			sprintf(sendbuffer,"register\n");
		} else if (strcmp(buffer, "2") == 0){
			sprintf(sendbuffer,"login\n");
		} else if (strcmp(buffer, "3") == 0){
			sprintf(sendbuffer,"exit\n");
			is_running = false;
		} else {
			sprintf(sendbuffer,"command not right\n");
		}

		printf("%s\n", buffer);
		send(new_socket, sendbuffer, 1024 ,0);
	}

	goodbye_ui(sendbuffer);
	send(new_socket, sendbuffer, strlen(sendbuffer) ,0);
	return 0;
}
