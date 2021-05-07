#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define PORT 8080

typedef struct {
	char id[64];
	char pass[64];
} akun;

typedef struct {
	char path[64];
	char publisher[64];
	char year[64];
} buku;

FILE *akun_file;
akun akun_data[10001] = {};
int akun_data_size = 0;

char akun_input_id[64] = {0};
char akun_input_pass[64] = {0};
char akun_input_pass_confirm[64] = {0};

akun *logined_akun = NULL;

FILE *buku_file;
buku buku_data[10001] = {};
int buku_data_size = 0;

char buku_input_path[64] = {0};
char buku_input_publisher[64] = {0};
char buku_input_year[64] = {0};

void resetbuffer(char *s){
	memset(s, 0, sizeof(char) * 1024);
}

void read_buku_file(){
	char data_temp[256] = {};

	int i = 0;

	buku_file = fopen("files.tsv", "r");

	printf("Reading files.tsv data..\n");

	while (fscanf(buku_file, " %[^\n]", data_temp) != EOF){
		if (strlen(data_temp) > 0){
			strcpy(buku_data[i].path, strtok(data_temp, "\t"));
			strcpy(buku_data[i].publisher, strtok(NULL, "\t"));
			strcpy(buku_data[i].year, strtok(NULL, "\t"));

			printf("Buku:\npath : %s\npublisher : %s\nyear : %s\n\n", buku_data[i].path, buku_data[i].publisher, buku_data[i].year);

			i++;
		}
	}

	buku_data_size = i;

	fclose(buku_file);

	printf("Done Reading files.csv data.\n");
}

void read_akun_file(){
	char data_temp[256] = {};

	int i = 0;

	akun_file = fopen("akun.txt", "r");

	printf("Reading akun.txt data..\n");

	while (fscanf(akun_file, "%s", data_temp) != EOF){
		if (strlen(data_temp) > 0){
			strcpy(akun_data[i].id, strtok(data_temp, ":"));
			strcpy(akun_data[i].pass, strtok(NULL, ":"));

			printf("%s -> %s\n", akun_data[i].id, akun_data[i].pass);

			i++;
		}
	}

	akun_data_size = i;

	fclose(akun_file);

	printf("Done Reading akun.txt data.\n");
}

void append_akun_file(char *id, char *pass){
	akun_file = fopen("akun.txt", "a");

	printf("Writing akun.txt data..\n");

	fprintf(akun_file, "%s:%s\n", id, pass);

	fclose(akun_file);

	printf("Done Write akun.txt data.\n");
}

int login_akun(char *id, char *pass){
	int i = 0;
	for (;i < akun_data_size;i++){
		if (strcmp(akun_data[i].id, id) == 0){
			if (strcmp(akun_data[i].pass, pass) == 0){
				logined_akun = &akun_data[i];
				return 0; //success
			} else {
				return 1; //false pass
			}
		}
	}
	return 2; //not found
}

void register_akun(char *id, char *pass){
	strcpy(akun_data[akun_data_size].id, id);
	strcpy(akun_data[akun_data_size].pass, pass);
	akun_data_size++;
	append_akun_file(id, pass);
}

void logout_akun(){
	logined_akun = NULL;
}

void sapa_user_ui(char *s){
	if (logined_akun){
		resetbuffer(s);
		sprintf(s, "Hello, %s!\n\nInput 'help' to get list of command\n\n", logined_akun->id);
	}
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

void help_ui(char *s){
	resetbuffer(s);
	sprintf(s,"Help Mee!\n");
	sprintf(s + strlen(s), "\n");
}

int main(int argc, char const *argv[]) {
	//Preparation
	read_akun_file();
	read_buku_file();

	//Untuk konek
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char sendbuffer[1024] = { 0 };
	char tempbuffer[1024] = { 0 };

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

	//Mulai konek

	bool is_running = true;

	valread = read( new_socket , buffer, 1024);
	home_ui(sendbuffer);
	send(new_socket, sendbuffer, 1024 ,0);

	while (is_running){
		resetbuffer(sendbuffer);

		if (!logined_akun){ //if not login
			valread = read( new_socket , buffer, 1024);
			if (strcmp(buffer, "1") == 0){
				resetbuffer(sendbuffer);
				sprintf(sendbuffer,"Register - Enter your id and pass\nid : ");
				send(new_socket, sendbuffer, 1024 ,0);
				valread = read( new_socket , buffer, 1024);
				strcpy(akun_input_id, buffer);

				resetbuffer(sendbuffer);
				sprintf(sendbuffer,"pass : ");
				send(new_socket, sendbuffer, 1024 ,0);
				valread = read( new_socket , buffer, 1024);
				strcpy(akun_input_pass, buffer);

				resetbuffer(sendbuffer);
				sprintf(sendbuffer,"pass confirmation : ");
				send(new_socket, sendbuffer, 1024 ,0);
				valread = read( new_socket , buffer, 1024);
				strcpy(akun_input_pass_confirm, buffer);

				resetbuffer(sendbuffer);
				if (strcmp(akun_input_pass, akun_input_pass_confirm) == 0){
					if (login_akun(akun_input_id, akun_input_pass) == 2) { //kalau gagal login, user tidak ada maka register
						register_akun(akun_input_id, akun_input_pass);
						sprintf(sendbuffer,"Register success!\n\nPlease login to continue\n\n");
					} else {
						sprintf(sendbuffer,"Register failed!\n\nUser exists!\n\n");
					}
				} else {
					sprintf(sendbuffer,"Register failed!\n\nPass confirm not match!\n\n");
				}
				logout_akun();
				home_ui(sendbuffer + strlen(sendbuffer));
			} else if (strcmp(buffer, "2") == 0){
				resetbuffer(sendbuffer);
				sprintf(sendbuffer,"Login - Enter your id and pass\nid : ");
				send(new_socket, sendbuffer, 1024 ,0);
				valread = read( new_socket , buffer, 1024);
				strcpy(akun_input_id, buffer);

				resetbuffer(sendbuffer);
				sprintf(sendbuffer,"pass : ");
				send(new_socket, sendbuffer, 1024 ,0);
				valread = read( new_socket , buffer, 1024);
				strcpy(akun_input_pass, buffer);

				int login_status = login_akun(akun_input_id, akun_input_pass);
				if (login_status == 0){
					//sprintf(sendbuffer,"Login success!\n\n");
					continue;
				} else if (login_status == 1){
					sprintf(sendbuffer,"Password invalid!\n\n");
				} else if (login_status == 2){
					sprintf(sendbuffer,"User not found!\n\n");
				}

				home_ui(sendbuffer + strlen(sendbuffer));
				
			} else if (strcmp(buffer, "3") == 0){
				sprintf(sendbuffer,"exit\n");
				is_running = false;
				break;
			} else {
				home_ui(sendbuffer);
				sprintf(sendbuffer + strlen(sendbuffer),"command not right\n");
			}
		} else {
			//logined
			sprintf(sendbuffer, "\nLogin success!\n");
			
			sapa_user_ui(tempbuffer);
			strcat(sendbuffer, tempbuffer);

			send(new_socket, sendbuffer, 1024 ,0);

			while (logined_akun){
				valread = read(new_socket , buffer, 1024);
				
				if (strcmp(buffer, "add") == 0){
				
				} else if (strcmp(buffer, "download") == 0){
				
				} else if (strcmp(buffer, "delete") == 0){
				
				} else if (strcmp(buffer, "see") == 0){
				
				} else if (strcmp(buffer, "find") == 0){
				
				} else if (strcmp(buffer, "logout") == 0){
					logout_akun();
					sprintf(sendbuffer, "Logout success!\n\n");
					break;
				} else if (strcmp(buffer, "help") == 0){
					help_ui(sendbuffer);
				} else {
					sapa_user_ui(sendbuffer);
				}
				send(new_socket, sendbuffer, 1024 ,0);
			}
		}

		send(new_socket, sendbuffer, 1024 ,0);
	}

	goodbye_ui(sendbuffer);
	send(new_socket, sendbuffer, 1024 ,0);
	return 0;
}
