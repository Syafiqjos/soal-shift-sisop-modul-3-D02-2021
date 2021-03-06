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
#include <sys/stat.h>
#include <sys/types.h>
#define PORT 8080

struct sockaddr_in address;
int sock = 0, valread;
struct sockaddr_in serv_addr;
char sendbuffer[1024] = { 0 };
char buffer[1024] = {0};
char filebuffer[1024] = {0};

pthread_t receiver_thread;
bool is_running = false;

void reset_buffer(char *s){
	memset(s, 0, sizeof(char) * 1024);
}

int read_file(char *path){
	reset_buffer(filebuffer);
	struct stat st = {0};
	if (stat(path, &st) != -1) {
		FILE *file = fopen(path, "rb");
		fread(&filebuffer, sizeof(char), 1024, file);
		fclose(file);
		return 1;
	}
	return 0;
}

void write_file(char *path, char *content){
	FILE *file = fopen(path, "wb");
	fwrite(content, sizeof(char), 1024, file);
	fclose(file);
}

void *receiver_func(void *args){
	pthread_t id = pthread_self();
	if (pthread_equal(id, receiver_thread)){
		while (!is_running) sleep(2); //timeout
		while (is_running){
			//check incoming message
			valread = read( sock , buffer, 1024);
			if (strcmp(buffer, "Thank you for using our database!\n\n") == 0){
				is_running = false;
			} else if (strcmp(buffer, "[$TRANSFER_UPLOAD]") == 0) { //upload prep signal
				printf("Uploading..\n");
				sleep(1);
				valread = read( sock , buffer, 1024);
				if (!read_file(buffer)){
					sleep(1);
					sprintf(sendbuffer, "[$404_SIGNAL]");
					send(sock, sendbuffer, 1024, 0);
					continue;
				}
				printf("Done uploading.\n");
				send(sock , filebuffer , 1024 , 0);
				continue;
			} else if (strcmp(buffer, "[$TRANSFER_DOWNLOAD]") == 0){ //download prep signal
				printf("Downloading..\n");
				sleep(1);
				valread = read(sock, buffer, 1024); //path

				char *fileinit = buffer + strlen(buffer);
				char *filename = malloc(sizeof(char) * 128);
				while (true){
					if (*fileinit == '/'){
						++fileinit;
						break;
					} else if (fileinit == buffer){
						break;
					}
					--fileinit;
				}
				strcpy(filename, fileinit);

				sleep(1);
				valread = read(sock, buffer, 1024); //content

				printf("writing data..\n");
				write_file(filename, buffer);
				printf("Done downloading.\n");

				free(filename);
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
