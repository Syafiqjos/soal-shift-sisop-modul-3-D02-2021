#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

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

int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[1024] = { 0 };
char sendbuffer[1024] = { 0 };
char tempbuffer[1024] = { 0 };
char filebuffer[1024] = { 0 };

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

void make_directory(char *s){
	mkdir(s, 0700);
}

void resetbuffer(char *s){
	memset(s, 0, sizeof(char) * 1024);
}

void send_message(char *s){
	resetbuffer(sendbuffer);
	sprintf(sendbuffer,"%s",s);
	send(new_socket, sendbuffer, 1024 ,0);
}

void receive_message(){
	valread = read(new_socket , buffer, 1024);
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


void append_buku_file(char *publisher, char *year, char *path){
	buku_file = fopen("files.tsv", "a");

	printf("Writing files.tsv data..\n");

	fprintf(buku_file, "%s\t%s\t%s\n", path, publisher, year);

	fclose(buku_file);

	printf("Done Write files.tsv data.\n");
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

void read_file(char *path){
	FILE *file = fopen(path, "r");
	fread(&filebuffer, sizeof(char), 1024, file);
	fclose(file);
}

void write_file(char *path, char* content){
	FILE *file = fopen(path, "w");
	fwrite(content, sizeof(char), 1024, file);
	fclose(file);
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

void sapa_user_ui(){
	if (logined_akun){
		char *s = tempbuffer;
		resetbuffer(s);
		sprintf(s, "Hello, %s!\n\nInput 'help' to get list of command\n\n", logined_akun->id);

		send_message(tempbuffer);
	}
}

void home_ui(){
	char *s = tempbuffer;
	resetbuffer(s);
	sprintf(s,"Welcome to database.\n");
	sprintf(s + strlen(s), "Input provided number to continue:\n");
	sprintf(s + strlen(s), "1. Register\n");
	sprintf(s + strlen(s), "2. Login\n");
	sprintf(s + strlen(s), "3. Exit\n");
	sprintf(s + strlen(s), "\n");

	send_message(tempbuffer);
}

void goodbye_ui(){
	char *s  = tempbuffer;
	resetbuffer(s);
	sprintf(s,"Thank you for using our database!\n");
	sprintf(s + strlen(s), "\n");

	send_message(tempbuffer);
}

void help_ui(){
	char *s = tempbuffer;
	resetbuffer(s);
	sprintf(s,"Help Mee!\n");
	sprintf(s + strlen(s), "\n");
	
	send_message(tempbuffer);
}

int main(int argc, char const *argv[]) {
	//Preparation
	make_directory("./FILES");
	read_akun_file();
	read_buku_file();

	//Untuk konek

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

	receive_message();
	home_ui(sendbuffer);

	while (is_running){
		if (!logined_akun){ //if not login
			receive_message();
			if (strcmp(buffer, "1") == 0){
				send_message("Register - Enter your id and pass\n> id :\n");
				receive_message();
				strcpy(akun_input_id, buffer);

				send_message("> pass :\n");
				receive_message();
				strcpy(akun_input_pass, buffer);

				send_message("> pass confirmation :\n");
				receive_message();
				strcpy(akun_input_pass_confirm, buffer);

				if (strcmp(akun_input_pass, akun_input_pass_confirm) == 0){
					if (login_akun(akun_input_id, akun_input_pass) == 2) { //kalau gagal login, user tidak ada maka register
						register_akun(akun_input_id, akun_input_pass);
						send_message("Register success!\n\nPlease login to continue\n\n");
					} else {
						send_message("Register failed!\n\nUser exists!\n\n");
					}
				} else {
					send_message("Register failed!\n\nPass confirm not match!\n\n");
				}
				logout_akun();
				home_ui();
			} else if (strcmp(buffer, "2") == 0){
				send_message("Login - Enter your id and pass\n> id :\n");
				receive_message();
				strcpy(akun_input_id, buffer);

				send_message("> pass :\n");
				receive_message();
				strcpy(akun_input_pass, buffer);

				int login_status = login_akun(akun_input_id, akun_input_pass);
				if (login_status == 0){
					send_message("Login success!\n\n");
				} else if (login_status == 1){
					send_message("Password invalid!\n\n");
				} else if (login_status == 2){
					send_message("User not found!\n\n");
				}

				home_ui();
				
			} else if (strcmp(buffer, "3") == 0){
				send_message("exit\n");
				is_running = false;
				break;
			} else {
				send_message("command not right\n");
				home_ui();
			}
		} else {
			//logined
			send_message("\nLogin success!\n");
			
			sapa_user_ui();

			while (is_running && logined_akun){
				receive_message();
				
				if (strcmp(buffer, "add") == 0){
					send_message("Insert book data!\n\n");
					send_message("Publisher:\n");
					receive_message();
					strcpy(buku_input_publisher, buffer);

					send_message("Tahun Publikasi:\n");
					receive_message();
					strcpy(buku_input_year, buffer)
						;
					send_message("Filepath:\n");
					receive_message();

					char *fileinit = buffer + strlen(buffer);
					char *filename = malloc(sizeof(char) * 64);

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
					strcpy(buku_input_path, filename);

					send_message("[$TRANSFER_UPLOAD]");
					sleep(2);
					send_message(buffer);
					receive_message(); //read data
					//printf("%s\n", buffer);
					
					printf("%s\n", filename);
					printf("%s\n", buffer);

					strcpy(filebuffer, buffer);
					sprintf(tempbuffer, "FILES/%s", filename); //nama file

					write_file(tempbuffer, filebuffer);
					append_buku_file(buku_input_publisher, buku_input_year, buku_input_path);

					free(filename);

					send_message("File Upload Success!\n");
				} else if (strcmp(buffer, "download") == 0){
					
				} else if (strcmp(buffer, "delete") == 0){
				
				} else if (strcmp(buffer, "see") == 0){
				
				} else if (strcmp(buffer, "find") == 0){
				
				} else if (strcmp(buffer, "logout") == 0){
					logout_akun();
					send_message("Logout success!\n\n");
					break;
				} else if (strcmp(buffer, "help") == 0){
					help_ui();
				} else {
					sapa_user_ui();
				}
			
			}
			home_ui();
		}
	}

	goodbye_ui();
	return 0;
}
