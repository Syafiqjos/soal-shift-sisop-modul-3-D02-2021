#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/wait.h>
#define PORT 8080

struct sockaddr_in address;
int sock = 0, valread;
struct sockaddr_in serv_addr;
char sendbuffer[1024] = { 0 };
char buffer[1024] = {0};

pthread_t receiver_thread;
bool is_running = false;

void *receiver_func(void *args){
	pthread_t id = pthread_self();
	if (pthread_equal(id, receiver_thread)){
		while (!is_running) sleep(2); //timeout
		while (is_running){
			//check incoming message
			valread = read( sock , buffer, 1024);
			if (strcmp(buffer, "Thank you for using our database!\n\n") == 0){
				is_running = false;
			} else if (strcmp(buffer, "[$TRANSFER_UPLOAD]") == 0) {
				valread = read( sock , buffer, 1024); //read file then upload
				continue;
			} else if (strcmp(buffer, "[$TRANSFER_DOWNLOAD]") == 0){
				valread = read( sock , buffer, 1024); //read file then download
				continue;
			}

			if (strlen(buffer) > 0){
				printf("%s", buffer);
			} else {
				sleep(1);
			}
		}
		exit(0);
	}
}

int main(int argc, char const *argv[]) {
	// create receiver thread
	int err = pthread_create(&receiver_thread, NULL, receiver_func,NULL);
	if (err){
		printf("Create receiver failed.\n");
	} else {
		printf("Thread starting receiving..\n");
	}

	// socket programming
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	is_running = true;

	send(sock , sendbuffer , 1024 , 0 );
	//valread = read( sock , buffer, 1024);
	printf("%s\n",buffer );

	while (is_running){
		//Input response buffer
		scanf("%s", sendbuffer);
		send(sock , sendbuffer , 1024 , 0 );
	}
	return 0;
}
