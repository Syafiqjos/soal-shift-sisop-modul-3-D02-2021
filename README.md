# soal-shift-sisop-modul-3-D02-2021
- Jessica Tasyanita (05111940000043)
- Ahmad Syafiq Aqil Wafi (05111940000089)
- Maximilian H M Lingga (05111940000092)

## Soal 1
### Tujuan
Membuat suatu system server dan client menggunakan socket programming yang dapat menerima multi-connection untuk melakukan tugas penyimpanan database buku.

### 1A. Membuat system server dan client dengan socket programming yang dapat menerima multi-connection 

#### Source Code Client
1. Code client fokus pada penerimaan input user dan menggunkan thread untuk menerima data atau pesan yang dikirim dari server. Dengan dipisah menggunakan thread seperti ini maka pesan dapat selalu ditampilkan tanpa harus menunggu input user secara asynchronus. Sesaat user mengirim pesan maka pesan data tersebut dikirim ke server.
```c
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
```

```c
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
```
#### Cara pengerjaan
1. Pada dasarnya, programming socket yang digunakan sama seperti modul, tetapi terdapat beberapa modifikasi.
2. Membuat thread untuk fungsi `receiver_func` yang digunakan untuk menerima data yang dikirim server. Sehingga data yang dikirim tidak harus menunggu user selesai mengirim data ke server.
3. Client fokus melakukan input dan menerima data dari server, tidak ada fungsi atau method yang explisit karena semua hal tersebut akan dilakukan pada server. Dapat dilihat pada `while (is_runnig)` hanya terdapat input dari user dan send data `sendbuffer` ke server.
4. Untuk membuat fungsi `receiver_func` melakukan check thread dulu dengan `pthread_equal` jika sesuai maka lanjutkan program.
5. Karena thread pada `receiver_func` selalu berjalan sehingga dapat selalu menerima data dari server, maka `receiver_func` menggunakan `while (is_running)` sehingga thread akan berhenti ketika `is_running` bernilai false.
6. Terdapat beberapa kondisi special yang digunakan sebagai indikasi bahwa pada server telah melakukan logout / exit, indikasi server akan menerima upload file dan indikasi client akan menerima download file. Hal ini hanya akan diatur server, sehingga client akan menerima indikator yang diberikan tersebut dan memutuskan apa yang harus dilakukan pada thread.
7. Download file dan Upload file tetap menggunakan jalur yang sama dengan pengiriman dan penerimaan pesan, untuk itulah indikator yang dijelaskan sebelumnya akan sangat dibutuhkan.

#### Source Code Server

```c
void write_file(char *path, char* content){
	FILE *file = fopen(path, "wb");
	fwrite(content, sizeof(char), 1024, file);
	fclose(file);
}
```

```c
void read_akun_file(){
	char data_temp[256] = {};

	int i = 0;

	akun_file = fopen("akun.txt", "r");

	printf("Reading akun.txt data..\n");

	while (fscanf(akun_file, " %[^\n]", data_temp) != EOF){
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
```

```c
void append_akun_file(char *id, char *pass){
	akun_file = fopen("akun.txt", "a");

	printf("Writing akun.txt data..\n");

	fprintf(akun_file, "%s:%s\n", id, pass);

	fclose(akun_file);

	printf("Done Write akun.txt data.\n");
}
```


```c
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
```

```c
void register_akun(char *id, char *pass){
	strcpy(akun_data[akun_data_size].id, id);
	strcpy(akun_data[akun_data_size].pass, pass);
	akun_data_size++;
	append_akun_file(id, pass);
}
```

```c
//reconnecting client

	while(true){
	
	printf("Processing..\n");

	sleep(1);

	printf("Waiting Connection..\n");

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	//Mulai konek

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
				//is_running = false;
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
					char *filename = malloc(sizeof(char) * 256);

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
					//
					if (strcmp(buffer, "[$404_SIGNAL]") == 0){
						send_message("File not found on local client!\n");
						sapa_user_ui();
						continue;
					}
					
					printf("%s\n", filename);
					printf("%s\n", buffer);

					strcpy(filebuffer, buffer);
					sprintf(tempbuffer, "FILES/%s", filename); //nama file

					write_file(tempbuffer, filebuffer);
					append_buku_file(buku_input_publisher, buku_input_year, buku_input_path);
					
					audit_log(1, filename);

					free(filename);

					send_message("File Upload Success!\n");
				} else if (strcmp(buffer, "download") == 0){
					send_message("Prepare to download. Insert server path!\n");
					send_message("path :\n");

					receive_message();
					strcpy(buku_input_path, buffer);

					if (read_file(buku_input_path)){

						printf("Trnasfer download\n");
						send_message("[$TRANSFER_DOWNLOAD]");

						printf("Waiting send path\n");
						sleep(2);
						send_message(buku_input_path);

						printf("CONTENT : %s\n", filebuffer);
	
						printf("Waiting send content\n");
						sleep(2);
						send_message(filebuffer);
					} else {
						send_message("File not found on server!\n");
					}

				} else if (strcmp(buffer, "delete") == 0){
					send_message("Delete file menu. Insert server path!\npath :\n");
					receive_message();
	
					delete_buku_file(buffer);
				} else if (strcmp(buffer, "see") == 0){
					read_buku_file(true);
				} else if (strcmp(buffer, "find") == 0){
					send_message("Find book menu. Input pattern to find book contains provided pattern!\npattern :\n");
					receive_message();

					find_buku_file(buffer);
				} else if (strcmp(buffer, "logout") == 0){
					logout_akun();
					send_message("Logout success!\n\n");
					break;
				} else if (strcmp(buffer, "exit") == 0){
					logout_akun();
					is_running = false;
					break;
				} else if (strcmp(buffer, "help") == 0){
					help_ui();
				}
				sapa_user_ui();
			
			}
			home_ui();
		}
	}

	goodbye_ui();
	resetbuffer(sendbuffer);
	printf("bye\n");
	}
```
#### Cara pengerjaan

1. Pada dasarnya, programming socket yang digunakan sama seperti modul, tetapi terdapat beberapa modifikasi.
2. Server melakukan seluruh aplikasi database yang digunakan, terdapat interpreter yang terjadi pada program server yang input tersebut dikirim dari aplikasi client.
3. Server memiliki 2 state utama, yaitu belum login dan telah login. Sehingga dibuat suatu sistem akun yang menyimpan informasi user yang sedang login saat ini.
4. Penyimpanan informasi login disimpan pada file yang bernama `akun.txt`. Untuk membuat kode konsisten, jika file ini tidak ada maka akan membuat file `akun.txt` menggunakan fungsi `write_file()` pada awal program.
5. Jika `akun.txt` ada maka kita bisa gunakan fungsi `read_akun_file()` untuk mendapatkan informasi akun dari `akun.txt`.
6. Ketika interpreter memutuskan untuk membuat akun baru maka dilakukan append pada `akun.txt` sebuah id dan password nya menggunakan fungsi `append_akun_file()`.
7. Ketika user berhasil login maka user akan dipindah ke halaman selanjutnya yaitu halaman untuk user yang berhasil login / interpreter selanjutnya.
8. Membuat fungsi `registrasi_akun` untuk mempermudah melakukan registrasi akun baru pada interpreter yang diinput client.
9. Membuat fungsi `login_akun` untuk melakukan login sesuai dengan id dan password yang diberikan client.

#### Contoh Output Program

Percobaan multi-connection, pada kiri adalah server, kanan atas client yang berhasil connect, kiri bawah client yang mencoba connect dan masih menunggu hingga client1 exit.

![image](https://user-images.githubusercontent.com/16128257/119264470-91b3ab80-bc0d-11eb-9cf0-7b4497853af0.png)

Percobaan register dan login client.

![image](https://user-images.githubusercontent.com/16128257/119264540-dc352800-bc0d-11eb-8208-4b4dced489b2.png)

Hasil akun.txt

![image](https://user-images.githubusercontent.com/16128257/119264575-fa028d00-bc0d-11eb-8fc9-3ae3608cdd35.png)

#### Kendala
- Untuk setiap pengiriman file dan pengiriman pesan yang terjadi pada server maupun client diberikan timout sebesar 1 - 2 detik. Hal ini dilakukan untuk meminimalisasikan error kebocoran data yang dapat terjadi. Terdapat suatu hal yang membuat data yang dikirim sama dengan pesan yang dikirim jika tidak diberikan timeout atau jeda.
- Untuk melakukan multi-connection hal yang perlu dilakukan adalah melakukan listen ulang pada client yang baru, bukan melakukan bind ulang. Karena jika socket melakukan bind ulang terdapat error berupa Port / Address is in use meskipun pada proses yang sama.

### 1B. Membuat system database informasi buku
#### Source Code

```c
typedef struct {
	char path[256];
	char publisher[256];
	char year[256];
} buku;
```

```c
void make_directory(char *s){
	mkdir(s, 0700);
}
```

```c
void make_file(char *path){
	FILE *file = fopen(path, "w");
	fclose(file);
}
```

```c
void read_buku_file(bool show){
	char data_temp[1024 * 8] = {};

	int i = 0;

	buku_file = fopen("files.tsv", "r");

	printf("Reading files.tsv data..\n");

	while (fscanf(buku_file, " %[^\n]", data_temp) != EOF){
		if (strlen(data_temp) > 0){
			printf("Original : %s\n", data_temp);
			strcpy(buku_data[i].path, strtok(data_temp, "\t"));
			strcpy(buku_data[i].publisher, strtok(NULL, "\t"));
			strcpy(buku_data[i].year, strtok(NULL, "\t"));

			char *ptr = buku_data[i].path + strlen(buku_data[i].path);
			while (ptr != buku_data[i].path && *ptr != '/'){
				--ptr;
			}

			if (ptr != buku_data[i].path){
				++ptr;
			}

			strcpy(buku_input_name, ptr);
			//strcpy(buku_input_ext, strtok(buku_input_name, "."));
			ptr = buku_input_name + strlen(buku_input_name);

			while (ptr != buku_input_name && *ptr != '.'){
				--ptr;
			}

			if (ptr != buku_input_name){
				++ptr;
			}

			strcpy(buku_input_ext, ptr);


			//strcpy(buku_input_name, buku_data[i].path);
			//strcpy(buku_input_ext, strtok(buku_input_name, "."));

			printf("Buku:\nname : %s\npublisher : %s\nyear : %s\nextension : %s\npath : %s\n\n", buku_input_name, buku_data[i].publisher, buku_data[i].year, buku_input_ext, buku_data[i].path);
			if (show){
				sprintf(tempbuffer,"Buku:\nname : %s\npublisher : %s\nyear : %s\nextension : %s\npath : %s\n\n", buku_input_name, buku_data[i].publisher, buku_data[i].year, buku_input_ext, buku_data[i].path);
				send_message(tempbuffer);
			}

			i++;
		}
	}

	buku_data_size = i;

	fclose(buku_file);

	printf("Done Reading files.csv data.\n");
}
```

```c
void write_buku_file(){
	buku_file = fopen("files.tsv", "w");

	int i = 0;
	for(;i < buku_data_size;i++){
		if (buku_data[i].path[0] != 0){
			fprintf(buku_file, "%s\t%s\t%s\n", buku_data[i].path, buku_data[i].publisher, buku_data[i].year);
		}
	}

	fclose(buku_file);

	read_buku_file(false);
}
```

```c
	//Preparation
	make_directory("./FILES");
	
	struct stat st = {0};
	if (stat("akun.txt", &st) == -1){
		make_file("akun.txt");
	}
	
	read_buku_file(false);
```

#### Cara Pengerjaan
1. Membuat fungsi `make_directory` yang didalamnya terdapat fungsi `mkdir` sehingga tidak perlu lagi memberikan permission secara berulang - ulang.
2. Fungsi `make_directory` digunakan untuk membuat directory `FILES` seperti yang diperintahkan. Fungsi ini dipanggil saat awal program berjalan.
3. Pada awal program juga memanggil fungsi `make_file` untuk membuat file `files.tsv` kosong jika file belum ada.
4. Membuat fungsi `read_buku_file` untuk membaca file `files.tsv` yang telah dibuat atau yang telah ada. Fungsi ini akan mengisi array of `buku` yang telah dibuat. Fungsi ini dipanggil pada awal program serta jika terdapat refresh pada `files.tsv` sehingga array pada program sesuai dengan file ini.
5. Membuat fungsi `write_buku_file` untuk melakukan write file `files.tsv` sesuai dengan array of `buku` yang ada pada program.
6. Membuat fungsi `append_buku_file` ini dipanggil ketika client melakukan registrasi user baru pada program. `fopen` append digunakan untuk mempermudah penambahan pada file yang telah ada, sehingga tidak kesulitan atau tidak seberat `write_buku_file` yang melakukan penulisan ulang buku.

#### Contoh Output
Catatan : Contoh output setelah command add dari client.

![image](https://user-images.githubusercontent.com/16128257/119265010-6762ed80-bc0f-11eb-9ee9-8a212cfea538.png)

![image](https://user-images.githubusercontent.com/16128257/119265065-90837e00-bc0f-11eb-8086-df539cf52f6f.png)

#### Kendala
Tidak ada kendala pada soal ini.

### 1C. Fitur agar client dapat menambahkan buku serta mengupload file buku
#### Source Code
#### Client
```c
			else if (strcmp(buffer, "[$TRANSFER_UPLOAD]") == 0) { //upload prep signal
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
			}
```

#### Server
```c
void write_file(char *path, char* content){
	FILE *file = fopen(path, "wb");
	fwrite(content, sizeof(char), 1024, file);
	fclose(file);
}
```

```c
void append_buku_file(char *publisher, char *year, char *path){
	buku_file = fopen("files.tsv", "a");

	printf("Writing files.tsv data..\n");

	char *abs_path = malloc(sizeof(char) * 1024);

	sprintf(abs_path, "%s/FILES/%s" ,getcwd(NULL, 0), path);

	//strcpy(abs_path, getcwd(NULL, 0));
	//strcat(abs_path, "/FILES/");
	//strcat(abs_path, path);

	printf("saved to abs_path : %s\n", abs_path);

	fprintf(buku_file, "%s\t%s\t%s\n", abs_path, publisher, year);

	free(abs_path);

	fclose(buku_file);

	read_buku_file(false);

	printf("Done Write files.tsv data.\n");
}
```

```c
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
					char *filename = malloc(sizeof(char) * 256);

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
					//
					if (strcmp(buffer, "[$404_SIGNAL]") == 0){
						send_message("File not found on local client!\n");
						sapa_user_ui();
						continue;
					}
					
					printf("%s\n", filename);
					printf("%s\n", buffer);

					strcpy(filebuffer, buffer);
					sprintf(tempbuffer, "FILES/%s", filename); //nama file

					write_file(tempbuffer, filebuffer);
					append_buku_file(buku_input_publisher, buku_input_year, buku_input_path);
					
					audit_log(1, filename);

					free(filename);

					send_message("File Upload Success!\n");
```

#### Cara Pengerjaan
1. Membuat fungsi `append_buku_file` untuk membuka file buku `fopen` dengan cara append untuk mempermudah melakukan write pada `files.tsv`.
2. Ketika memasukkan buku, client mengirim data informasi Publisher, Tahun dan Path pada client. Disini send dan receive diatur pada kondisi `add` saja seperti pada code, tidak menggunakan interpreter global seperti sebelum masuk kondisi `add`.
3. Ketika client mengirimkan `path` buku, maka client melakukan check pada file local client, jika file tidak ada maka client akan mengirim flag `[$404_SIGNAL]` sehingga server tahu jika file tidak ada pada client sehingga request `add` dibatalkan.
4. Namun ketika file ada maka client akan mengirim data file kepada server, sehingga jalur receive server akan terisi dengan data buku. Ketika file buku telah didapatkan, maka server menyimpan dengan menggunakan fungsi `write_file` pada path yang diberikan client dan content merupakan isi transfer file tersebut.
5. Path pada `write_file` akan disimpan pada folder `FILES` yang telah dibuat dengan nama file sama dengan nama file yang dikirim client.
6. Selanjutnya melakukan update pada array of `book` pada fungsi `append_buku_file` yang sekalian menambah informasi buku pada `files.tsv`.

#### Contoh Output

![image](https://user-images.githubusercontent.com/16128257/119264699-6b424000-bc0e-11eb-97dd-a27ea7ecb70e.png)

#### Kendala
1. Terdapat kendala saat melakukan upload file dari client karena hanya terdapat satu jalur untuk penerimaan pesan. Oleh karena itu kita harus menggunakan flag indikator `[$TRANSFER_UPLOAD]` dari client agar server mengetahui bahwa data yang akan dikirim selanjutnya merupakan sebuah data buku, bukan input dari client.
2. Juga ketika file tidak ditemukan menggunakan flag indikator `[$404_SIGNAL]` sehingga server tahu jika file path yang diberikan client tidak ada pada storage local client.

### 1D. Fitur agar client dapat mendownload buku dari server
#### Source Code
#### Client
```c
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
```

#### Server
```c
bool read_file(char *path){
	struct stat st = {0};
	char tempp[256] = {0};

	sprintf(tempp, "FILES/%s", path);

	if (stat(tempp, &st) == -1){
		return 0;
	}

	FILE *file = fopen(tempp, "rb");
	fread(filebuffer, sizeof(char), 1024, file);

	fclose(file);

	return 1;
}
```

```c
				else if (strcmp(buffer, "download") == 0){
					send_message("Prepare to download. Insert server path!\n");
					send_message("path :\n");

					receive_message();
					strcpy(buku_input_path, buffer);

					if (read_file(buku_input_path)){

						printf("Trnasfer download\n");
						send_message("[$TRANSFER_DOWNLOAD]");

						printf("Waiting send path\n");
						sleep(2);
						send_message(buku_input_path);

						printf("CONTENT : %s\n", filebuffer);
	
						printf("Waiting send content\n");
						sleep(2);
						send_message(filebuffer);
					} else {
						send_message("File not found on server!\n");
					}
```
#### Cara Pengerjaan
1. Membuat fungsi `read_file` untuk membaca file dan mengecheck apakah file ada atau tidak, jika ada maka fungsi akan mengisi `filebuffer` yang akan digunakan untuk mengirim data buku dari server ke client.
2. Pada saat kondisi download, melakukan check jika file ada pada server, jika tidak ada maka proses akan di skip dan kembali ke interpreter global.
3. Jika file ada maka server mengirim flag indikator `[$TRANSFER_DOWNLOAD]` sehingga client tahu jika data yang akan dikirim selanjutnya merupakan data buku bukan pesan biasa.
4. Client menerima data buku tersebut yang kemudian disimpan pada folder program dijalankan.
5. Terdapat timeout sekitar 2 detik menggunakan fungsi `sleep(2)`. Hal ini dilakukan agar jalur yang digunakan untuk pengiriman terdapat jeda, sehingga data yang akan dikirimkan tidak akan menumpuk.
6. Setelah file download berhasil maka server akan kembali pada interpreter global.

#### Contoh Output

Client

![image](https://user-images.githubusercontent.com/16128257/119264781-afcddb80-bc0e-11eb-8ddd-63ef38b96f26.png)

Server

![image](https://user-images.githubusercontent.com/16128257/119264787-b52b2600-bc0e-11eb-8d93-5d6a6f864d1d.png)

#### Kendala
1. Terdapat kendala saat melakukan download file dari server karena hanya terdapat satu jalur untuk penerimaan pesan. Oleh karena itu kita harus menggunakan flag indikator `[$TRANSFER_DOWNLOAD]` dari server agar client mengetahui bahwa data yang akan diterima selanjutnya merupakan sebuah data buku, bukan output dari server.

### 1E. Fitur agar client dapat menghapus data buku di server
#### Source Code
```c
void delete_buku_file(char *path){
	int found = -1;
	int i = 0;

	char *find_name;
	char *next_find_name;

	for (;i < buku_data_size;i++){
		find_name = buku_data[i].path + strlen(buku_data[i].path);

		while (find_name != buku_data[i].path){
			if (*find_name == '/'){
				find_name++;
				break;
			}
			find_name--;
		}
		printf("%s == %s\n", path, find_name);

		if (strcmp(path, find_name) == 0){
			found = i;
		}
	}

	if (found != -1){
		char file_name[512] = {0};
		char file_name_new[512] = {0};

		sprintf(file_name, "FILES/%s", buku_data[found].path);
		sprintf(file_name_new, "FILES/old-%s", buku_data[found].path);
		rename(file_name, file_name_new);

		buku_data[found].path[0] = 0;
		write_buku_file();

		audit_log(2, path);

		send_message("Book deleted successfully.\n");
	} else {
		send_message("Book not found\n");
	}
}
```

```c
void write_buku_file(){
	buku_file = fopen("files.tsv", "w");

	int i = 0;
	for(;i < buku_data_size;i++){
		if (buku_data[i].path[0] != 0){
			fprintf(buku_file, "%s\t%s\t%s\n", buku_data[i].path, buku_data[i].publisher, buku_data[i].year);
		}
	}

	fclose(buku_file);

	read_buku_file(false);
}
```

```c
				} else if (strcmp(buffer, "delete") == 0){
					send_message("Delete file menu. Insert server path!\npath :\n");
					receive_message();
	
					delete_buku_file(buffer);
				}
```

#### Cara Pengerjaan
1. Membuat fungsi `delete_buku_file` untuk mempermudah melakukan delete buku.
2. Client mengirim informasi tentang path buku yang akan dihapus dari `files.tsv`.
3. Melakukan pengecheckan pada array of `book`, jika terdapat `book` yang memiliki basename sama dengan nama file yang diberikan client maka data tersebut akan dihapus.
4. Karena menghapus buku pada `files.tsv` akan sulit, maka kami memutuskan untuk melakukan write ulang pada `files.tsv` menggunakan fungsi `write_buku_file`. Fungsi ini akan menulis file `files.tsv` sesuai dengan array of `book` yang ada pada program.
5. Sesuai dengan perintah, file buku yang akan dihapus tidak akan dihapus sepenuhnya, melainka melakukan rename dan ditambahkan `old-` pada depan file buku tersebut. Rename buku ini menggunakan fungsi `rename` yang default ada pada module yang diimport yang kami gunakan.

#### Contoh Output
Client

![image](https://user-images.githubusercontent.com/16128257/119264809-d4c24e80-bc0e-11eb-9c0d-c3260c6af06d.png)

Server

![image](https://user-images.githubusercontent.com/16128257/119264819-ddb32000-bc0e-11eb-85c3-bd6317700903.png)

#### Kendala
Tidak ada kendala pada soal ini.

### 1F. Fitur agar client dapat mendapat informasi setiap buku yang ada di server
#### Source Code
```c
void read_buku_file(bool show){
	char data_temp[1024 * 8] = {};

	int i = 0;

	buku_file = fopen("files.tsv", "r");

	printf("Reading files.tsv data..\n");

	while (fscanf(buku_file, " %[^\n]", data_temp) != EOF){
		if (strlen(data_temp) > 0){
			printf("Original : %s\n", data_temp);
			strcpy(buku_data[i].path, strtok(data_temp, "\t"));
			strcpy(buku_data[i].publisher, strtok(NULL, "\t"));
			strcpy(buku_data[i].year, strtok(NULL, "\t"));

			char *ptr = buku_data[i].path + strlen(buku_data[i].path);
			while (ptr != buku_data[i].path && *ptr != '/'){
				--ptr;
			}

			if (ptr != buku_data[i].path){
				++ptr;
			}

			strcpy(buku_input_name, ptr);
			//strcpy(buku_input_ext, strtok(buku_input_name, "."));
			ptr = buku_input_name + strlen(buku_input_name);

			while (ptr != buku_input_name && *ptr != '.'){
				--ptr;
			}

			if (ptr != buku_input_name){
				++ptr;
			}

			strcpy(buku_input_ext, ptr);


			//strcpy(buku_input_name, buku_data[i].path);
			//strcpy(buku_input_ext, strtok(buku_input_name, "."));

			printf("Buku:\nname : %s\npublisher : %s\nyear : %s\nextension : %s\npath : %s\n\n", buku_input_name, buku_data[i].publisher, buku_data[i].year, buku_input_ext, buku_data[i].path);
			if (show){
				sprintf(tempbuffer,"Buku:\nname : %s\npublisher : %s\nyear : %s\nextension : %s\npath : %s\n\n", buku_input_name, buku_data[i].publisher, buku_data[i].year, buku_input_ext, buku_data[i].path);
				send_message(tempbuffer);
			}

			i++;
		}
	}

	buku_data_size = i;

	fclose(buku_file);

	printf("Done Reading files.csv data.\n");
}
```

```c
				} else if (strcmp(buffer, "see") == 0){
					read_buku_file(true);
				}
```

#### Cara Pengerjaan
1. Membuat fungsi `read_buku_file` untuk melakukan read ulang pada `files.tsv` dan melakukan output informasi - informasi buku yang ada didalamnya.
2. Kami memutuskan untuk menggunakan fungsi `read_buku_file` dan tidak membuat fungsi baru karena menurut kami hal ini mirip dengan `read_buku_file` yang sebelumnya. Hal ini dibedakan dengan parameter `show` yang bernilai true saat pemanggilan `see` dari client. Argument show yang bernilai true akan mengirimkan informasi data buku kepada client.
3. Menggunakan fungsi `strtok` untuk melakukan split dari line raw `files.tsv` yang dipisahkan `\t`.
4. Memasukkan hasil split pada variable yang sesuai lalu mengirimkan output pada client.
5. Client hanya akan mengoutputkan pesan yang dikirim dari server seperti biasanya.

#### Kendala
Tidak ada kendala untuk soal ini.

#### Contoh Output

![image](https://user-images.githubusercontent.com/16128257/119264877-00453900-bc0f-11eb-95d9-702fa27ccd50.png)

### 1G. Fitur agar client dapat mendapat informasi setiap buku sesuai dengan filter yang ada di server
#### Source Code
```c
void find_buku_file(char *pattern){
	int i = 0;
	for (;i < buku_data_size;i++){
		if (strstr(buku_data[i].path, pattern) || strstr(buku_data[i].publisher, pattern) || strstr(buku_data[i].year, pattern)){
				sprintf(tempbuffer,"Buku:\npath : %s\npublisher : %s\nyear : %s\n\n", buku_data[i].path, buku_data[i].publisher, buku_data[i].year);
				send_message(tempbuffer);
		}
	}
}
```

```c
				} else if (strcmp(buffer, "find") == 0){
					send_message("Find book menu. Input pattern to find book contains provided pattern!\npattern :\n");
					receive_message();

					find_buku_file(buffer);
				}
```
#### Cara Pengerjaan
1. Membuat fungsi `find_buku_file` untuk mempermudah pencarian buku sesuai dengan pattern yang diberikan.
2. Saat masuk pada kondisi find, maka menjalankan fungsi `find_buku_file` dengan argument yang diberikan oleh client.
3. Fungsi `find_buku_file` bekerja dengan cara melakukan iterasi pada array of `book` yang dimana pada setiap iterasi tersebut mencari data buku yang cocok dengan pattern yang diberikan. Jika terdapat pattern yang sesuai maka kirim output tersebut kepada client.
4. Untuk melakukan check pattern dapat menggunakan fungsi `strstr`. Fungsi ini melakukan check apakah pada suatu string terdapat substring atau tidak. Jika ada maka akan mengembalikan indeks substring tersebut.
5. Jika tidak ada buku yang cocok sama sekali maka tidak mengirim output apapun kepada client.

#### Contoh Output

![image](https://user-images.githubusercontent.com/16128257/119264921-1fdc6180-bc0f-11eb-80f9-70b1baaf5251.png)

#### Kendala
Tidak ada kendala pada soal ini.

### 1H. Fitur agar server dapat menyimpan log perubahan file yang dilakukan client
#### Source Code
```c
akun *logined_akun = NULL;
```

```c
void audit_log(int mode, char *path){
	if (logined_akun){
		FILE *file = fopen("running.log", "a");
		if (mode == 1){
			fprintf(file, "Tambah : %s (%s:%s)\n", path, logined_akun->id, logined_akun->pass);
		} else if (mode == 2){
			fprintf(file, "Hapus : %s (%s:%s)\n", path, logined_akun->id, logined_akun->pass);
		}
		fclose(file);
	}
}
```

Pada Fungsi delete_book_file
```c
if (found != -1){
		char file_name[512] = {0};
		char file_name_new[512] = {0};

		sprintf(file_name, "FILES/%s", buku_data[found].path);
		sprintf(file_name_new, "FILES/old-%s", buku_data[found].path);
		rename(file_name, file_name_new);

		buku_data[found].path[0] = 0;
		write_buku_file();

		audit_log(2, path);

		send_message("Book deleted successfully.\n");
	} else {
		send_message("Book not found\n");
	}
```

Pada saat add book
```c
					strcpy(filebuffer, buffer);
					sprintf(tempbuffer, "FILES/%s", filename); //nama file

					write_file(tempbuffer, filebuffer);
					append_buku_file(buku_input_publisher, buku_input_year, buku_input_path);
					
					audit_log(1, filename);

					free(filename);

					send_message("File Upload Success!\n");
```

```c
	if (stat("running.log", &st) == -1){
		make_file("running.log");
	}
```

#### Cara Pengerjaan
1. Membuat variable `logined_akun` untuk melakukan track akun dengan id dan pass apa yang sedang login.
2. Membuat fungsi `audit_log` untuk mempermudah melakukan audit pada `running.log`
3. Pada saat awal program dijalankan, menjalankan `make_file("running.log")` untuk membuat file `running.log`.
4. Pada saat user melakukan add book maka akan menjalankan fungsi `audit_log` dengan mode 1, yaitu mode add.
5. Pada saat user melakukan delete book maka akan menjalankan fungsi `audit_log` dengan mode 2, yaitu mode delete.
6. Pada fungsi `audit_log` terdapat mode untuk mempermudah logging. Jika mode adalah 1 maka akan menulis log untuk add, sedangkan mode 2 akan menulis log delete.
7. Untuk menuliskan informasi user kita akan memanfaatkan variable `logined_akun` yang telah dibuat, sehingga pada saat melakukan logging file kita dapat menggunakan variable ini untuk mendapatkan `id` dari user yang sedang login.

#### Contoh Output
![image](https://user-images.githubusercontent.com/16128257/119264949-3aaed600-bc0f-11eb-94ec-655790ea3e4f.png)

#### Kendala
Tidak ada kendala pada soal ini.

## Soal 2
### Tujuan
Melakukan operasi perkalian dan faktorial pada dua matrix menggunakan shared memory dan melakukan pengecekan 5 proses teratas
### 2A. Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
#### Cara pengerjaan 
1. Deklarasi variabel
```
#define x 4
#define y 3
#define z 6

// const int matrix1[x][y] = {{4, 1, 4}, {2, 1, 3}, {4, 2, 2}, {1, 1, 4}};
// const int matrix2[y][z] = {{2, 1, 3, 2, 0, 3}, {1, 4, 4, 0, 0, 2}, {1, 1, 0, 1, 2, 1}};
int matrix1[x][y];
int matrix2[y][z];
int result[x][z];

pthread_t tid[x*z];
pthread_attr_t attr;
int iret[x*z];
key_t key = 1234;
int *matrix;
int cnt=0;

void *hitung(void *arguments);

struct arg_struct{
    int arg1;
    int arg2;
};
```
2. Melakukan input matrix
```
printf("Input Matrix 1\n");
    for (int i = 0; i < x; ++i) {
      for (int j = 0; j < y; ++j) {
         printf("Enter a%d%d: ", i + 1, j + 1);
         scanf("%d", &matrix1[i][j]);
      }
    }

    printf("Input Matrix 2\n");
    for (int i = 0; i < y; ++i) {
      for (int j = 0; j < z; ++j) {
         printf("Enter a%d%d: ", i + 1, j + 1);
         scanf("%d", &matrix2[i][j]);
      }
    }
```
3. Mengalokasikan _Shared Memory_ agar hasil dari perkalian matrix dapat digunakan pada soal b
```
int shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666);
    matrix = shmat(shmid, 0, 0);
```
4. Menggunakan thread untuk melakukan perhitungan matrix, dan memanggil fungsi hitung untuk menghitung perkalian matrix
```
void call_thread (){
    for(int i=1; i<x+1; i++){
        for(int j=1; j<z+1; j++){
            struct arg_struct *args = (struct arg_struct *) malloc(sizeof(struct arg_struct));
            args->arg1 = i-1;
            args->arg2 = j-1;
            pthread_attr_init(&attr);
            iret[cnt] = pthread_create(&tid[cnt], &attr, hitung, args);
            if(iret[cnt]){
                fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[cnt]);
                exit(EXIT_FAILURE);
            }
            cnt++;
        }
    }
}
```
5. Melakukan perhitungan perkalian dalam sebuah fungsi
```
void *hitung(void *arguments){
    struct arg_struct *args = arguments;

    int temp = 0;
    int d1=args->arg1;
    int d2=args->arg2;

    for(int i=0; i<y; i++){
        temp = temp + (matrix1[d1][i] * matrix2[i][d2]);
    }

    result[d1][d2] = temp;
    pthread_exit(0);
}
```
6. Menggunakan thread join untuk menampilkan hasil dan memasukkan hasil ke variabel yang digunakan untuk _Shared Memory_
```
void join_thread(){
    for(int i=0; i<x; i++){
        for(int j=0; j<z; j++){
            pthread_join(tid[cnt], NULL);
            printf("%d\t", result[i][j]);
            matrix[cnt] = result[i][j];
            cnt++;
        }
        printf("\n");
    }
}
```
7. Men-_detatche_ lokasi segmed shared memory pada alamat spesifik
```
shmdt(matrix)
```
#### Hasil Running Program

![image](https://user-images.githubusercontent.com/62937814/118835931-a03c5300-b8ed-11eb-9c9c-a104214327d2.png)

#### Kendala
1. Karena setiap run melakukan input matrix, agar tidak menghabiskan waktu maka menambahkan comment yang merupakan matrix yang telah memiliki isi agar bisa digunakan
```
// const int matrix1[x][y] = {{4, 1, 4}, {2, 1, 3}, {4, 2, 2}, {1, 1, 4}};
// const int matrix2[y][z] = {{2, 1, 3, 2, 0, 3}, {1, 4, 4, 0, 0, 2}, {1, 1, 0, 1, 2, 1}};
```
2. Untuk memudahkan deteksi index matrix yang sedang dioperasikan maka menambahkan struct bernama `arg_struct` untuk menyimpan index matrix yang sedang dioperasikan


### 2B. 
Menggunakan hasil dari soal A dan menambah sebuah matrix baru (Matrix B) kemudian melakukan operasi dengan ketentuan seperti berikut
```
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```
#### Cara pengerjaan 
1. Inisialisasi Variabel
```
#define x 4
#define z 6

pthread_t tid[x*z];
pthread_attr_t attr;
int iret[x*z];
key_t key = 1234;
int *matrix;
int cnt=0;
const int matrixB[x][z] = {{14, 2, 3, 8, 8, 10}, {7, 4, 8, 5, 14, 9}, {9, 2, 13, 5, 11, 2}, {8, 7, 10, 4, 10, 8}};
//const int matrixB[x][z] = {{1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}};
int matrixA[x][z];
// int matrixB[x][z];
int total=x*z;

void *faktorial(void *arguments);

struct arg_struct{
    int arg1;
    int arg2;
};
```
2. Mengalokasikan _Shared Memory_
```
int shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666);
    matrix = shmat(shmid, 0, 0);
```
3. Meng-_assign_ matrix dari _Shared Memory_ kepada matrix A
```
void assign_matrixA(){
    cnt=0;
    for(int i=0; i<x; i++){
        for(int j=0; j<z; j++){
            matrixA[i][j] = matrix[cnt];
            //printf("%d\t", matrixA[i][j]);
            cnt++;
        }
        //printf("\n");
    }
}
```
4. Input untuk Matrix B
```
printf("Input matrix B\n");
    //input matrixB
    for (int i = 0; i < x; i++) {
      for (int j = 0; j < z; j++) {
         printf("Enter a%d%d: ", i+1, j+1);
         scanf("%d", &matrixB[i][j]);
      }
   }
```
5. Menggunakan Thread untuk melakukan operasi perhitungan setiap _cell_ nya dan memanggil fungsi faktorial
```
 for(int i=1; i<x+1; i++){
        for(int j=1; j<z+1; j++){
            struct arg_struct *args = (struct arg_struct *) malloc(sizeof(struct arg_struct));
            args->arg1 = i-1;
            args->arg2 = j-1;
            pthread_attr_init(&attr);
            iret[cnt] = pthread_create(&tid[cnt], &attr, faktorial, args);
            if(iret[cnt]){
                fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[cnt]);
                exit(EXIT_FAILURE);
            }
            pthread_join(tid[cnt], NULL);
            cnt++;
        }
        printf("\n");
    }
```
6. Melakukan operasi perhitungan pada matrix
```
void *faktorial(void *arguments){
    struct arg_struct *args = arguments;

    int baris=args->arg1;
    int kolom=args->arg2;
    long long int hasil=1;

    if (matrixA[baris][kolom] == 0 || matrixB[baris][kolom] == 0){
        printf("0");
       // temp = 0;
    }
    else if(matrixA[baris][kolom]>=matrixB[baris][kolom]){
        //a!/(a-b)!
        int batas = matrixA[baris][kolom] - matrixB[baris][kolom];
        for (int i = matrixA[baris][kolom]; i > batas; i--)
        {
            hasil = hasil*i;
            //printf("%d ", i);
        }
        //printf("\n");
        printf("%lld", hasil);
        
    }
    else if(matrixB[baris][kolom]>matrixA[baris][kolom]){
        //a!
        for (int i = matrixA[baris][kolom]; i > 0; i--)
        {
            hasil = hasil * matrixA[baris][kolom];
            //printf("%d ", matrixA[baris][kolom]);
            matrixA[baris][kolom]--;
        }
        //printf("\n");
        printf("%lld", hasil);
    }
    printf("\t\t");
    pthread_exit(0);
}
```
Pada matrix A >= Matrix B menambahkan variabel batas untuk menjadi batas banyaknya faktorial dari matrix A

7. Memanggil pthread join
```
for(int i=0; i<total; i++){
        pthread_join(tid[i], NULL);
    }
```
8. Menutup _Shared Memory_
```
shmdt(matrix);
    shmctl(shmid, IPC_RMID, NULL);
```
#### Hasil Running Program
![image](https://user-images.githubusercontent.com/62937814/118836387-fe693600-b8ed-11eb-982c-364ef42d5ffe.png)

#### Kendala
1. Jika hasil operasi matrix hasilnya besar, outpunya kurang rapih.

### 2C. 
Membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes).

#### Cara pengerjaan 
1. Memanggil pipe
```
if (pipe(pipe1) == -1) {
    perror("bad pipe1");
    exit(1);
  }
```
2. Memanggil fungsi yang menjalankan command pertama
```
if ((pid = fork()) == -1) {
    perror("bad fork1");
    exit(1);
  } else if (pid == 0) {
    exec1();
  }
```
3. Menjalankan command pertama yaitu `ps aux` menggunakan `execlp`
```
void exec1() {
  dup2(pipe1[1], 1);

  close(pipe1[0]);
  close(pipe1[1]);

  execlp("ps", "ps", "aux", NULL);
  // exec gajalan, keluar
  perror("bad exec ps");
  _exit(1);
}
```
4. Menjalankan command kedua yaitu `head -5` menggunakan `execlp`
```
void exec2() {
  // input dari pipe1
  dup2(pipe1[0], 0);
  // output ke pipe2
  dup2(pipe2[1], 1);

  close(pipe1[0]);
  close(pipe1[1]);
  close(pipe2[0]);
  close(pipe2[1]);
 
  execlp("sort", "sort", "-nrk", "3,3", NULL);
  // exec gajalan, keluar
  perror("bad exec sort");
  _exit(1);
}
```
5. Menjalankan command ketiga yaitu `sort -nrk 3,3` menggunakan `execlp`
```
void exec3() {
  dup2(pipe2[0], 0);

  close(pipe2[0]);
  close(pipe2[1]);

  execlp("head", "head", "-5", NULL);

  perror("bad exec head");
  _exit(1);


  exit(0);
}
```
#### Kendala
Apabila dijalankan, program tidak bisa keluar dengan sendirinya, harus menggunakan `ctrl + C`
#### Hasil Running Program
![image](https://user-images.githubusercontent.com/62937814/118837814-5e141100-b8ef-11eb-9bc1-262929280cc9.png)

## Soal 3
### Tujuan
Membuat sebuah program untuk mengkategorikan file dan direktori yang tersedia.

### Cara Pengerjaan
Sebelum menjalankan fungsi main, kita terlebih dahulu membuat beberapa fungsi yang diperlukan dalam program

1. Menentukan opsi yang akan dijalankan dan membuat struct rec_args
```
// 0 == -f, 1 == -d
int mode = 0;

typedef struct {
	char *path;
	int inde;
} rec_args;
```
2. Membuat fungsi untuk mengambil ekstensi
 ```
 char *get_extension(char *filename) {
	if(filename[0] == '.') {
	    return "Hidden"; 
	}
	
	char *temp = strchr(filename, '.');
	
	if(!temp) {
		return "Unknown";
	}

    return temp + 1;
}
```
3. Membuat fungsi pembuat folder
```
//make folder
void make_directory (char *dir) {
	struct stat st;
	if (stat(dir, &st) == -1) {
		mkdir(dir, 0777);
	}
}
```
4. Membuat program pemindah file
```void *move_file(void *argv){
	char *path;
	int inde;
	char cwd[128], folder[128], folderpath[400], filepath[512]; 

	rec_args *argg = (rec_args *)argv;

	path = argg->path;
	inde = argg->inde;

       	getcwd(cwd, sizeof(cwd));

       	char* file = basename(path);
       	strcpy(folder, get_extension(file));
 
       	if (strcmp(folder, "Hidden") != 0 && strcmp(folder,"Unknown") != 0) {
	      	for(int i = 0; i < strlen(folder); i++) {
		     	folder[i] = tolower(folder[i]);
		}
	}
```
Untuk menjalankan opsi -f, kita juga membuat outputnya
```
sprintf(folderpath, "%s/%s", cwd, folder);

	make_directory(folderpath);

	sprintf(filepath, "%s/%s", folderpath, file);

	//printf("%s\n", filepath);

	int failed = rename(path , filepath); 
	
	if (mode == 0){
		if (!failed) {
			printf ( "File %d : Berhasil Dikategorikan\n",inde);
		}
	    	else {
		    	printf( "File %d : Sad, gagal :(\n", inde);
		}
	}
}
```
5. Membuat fungsi untuk menelusuri folder dan file di komputer
5a. Membuka folder
```
void rec(char *argv){
	int k = 2; 
	struct dirent *dp;
	DIR *dir = opendir(argv);
 

       pthread_t thread[1000];
 ```
 5b. Menelusuri dan membaca file (apabila langsung ditemukan file)
 ```
   while ((dp = readdir(dir)) != NULL) {
	     char path[512];
 
	     if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
		  if(dp->d_type == DT_REG) {
		      if (strcmp(dp->d_name, "soal3") != 0 && strcmp(dp->d_name, "soal3.c") != 0 && strcmp(dp->d_name, "a.out") != 0) {
			      sprintf(path, "%s/%s", argv, dp->d_name);
	
			      rec_args *argg = malloc(sizeof(rec_args));
			      argg->path = path;

			      pthread_create(&thread[k], NULL, move_file, (void *) argg);
			      pthread_join(thread[k], NULL);
			      k++;
		      }
	       	  }
```
Dalam mengecek file, kita perlu memisahkan file-file yang berhubungan dengan program pada soal ini (misalnya seperti soal3 dan soal3.c pada kode)
5c. Menelusuri dan membuka folder (jika masih ada folder didalam folder)
```
  else if(dp->d_type == DT_DIR) {
		      	  struct dirent *ep;
		   	  DIR *dp2 = opendir(argv);
		      	  char path2[512];
    
			  sprintf(path2, "%s/%s", argv, dp->d_name);
		      	  rec(path2);
		   	  closedir(dp2);
	       	  }
	     }
       }
       closedir(dir);
}
```
Setelah membuat fungsi-fungsi yang diperlukan, selanjutnya kita beranjak ke fungsi main.
### Cara Pengerjaan
1. Mengecek apakah opsi -f dan/atau -d diperlukan/tersedia
```
int main(int argc, char* argv[]) {
	if (argc == 0 || argc == 1){
		printf("Tidak ada argument -f atau -d ditemukan\n");
		return 0;
	}
```
![image](https://user-images.githubusercontent.com/16128257/119264216-68dee680-bc0c-11eb-8c04-517649d48a5a.png)
2. Menjalankan opsi -f
```

	if (strcmp(argv[1], "-f") == 0){
		pthread_t tid[1000];
		for(int i = 2; i < argc; i++){
			
			rec_args *argg = malloc(sizeof(rec_args));
			argg->path = argv[i];
			argg->inde = i - 1;

			pthread_create(&tid[i], NULL, move_file, (void *) argg); 
		}
	
		for(int j = 2; j < argc; j++)  {
			pthread_join(tid[j],NULL);
		}
	}
```
![image](https://user-images.githubusercontent.com/16128257/119264199-5795da00-bc0c-11eb-9da7-70b3690a1990.png)

catatan : terdapat typo pada tulisan sehingga file gagal dipindah ke kategori. Folder `test` dan `test2` merupakan folder dummy dan bukan yang kategorikan.

3. Menjalankan opsi -d
```c
else if(strcmp(argv[1], "-d") == 0){
		mode = 1;
		if(errno != 2) {
			rec(argv[2], true);
			printf("Direktori sukses disimpan!\n");
		}
		else {
		     	printf("Yah, gagal disimpan :(\n");
		}
	}
```

![image](https://user-images.githubusercontent.com/16128257/119264305-cd01aa80-bc0c-11eb-831c-00bb5436370b.png)

4.Menjalankan opsi *
```c
else {
		char cwd[128];
		getcwd(cwd, sizeof(cwd));

		mode = 1;

		char* folder = dirname(argv[1]);

		char newfolder[256];

		sprintf(newfolder, "%s/%s", cwd, folder);

		rec(newfolder, false);

		printf("Files sukses dikategorikan!\n");
	}
```

![image](https://user-images.githubusercontent.com/16128257/119264336-f1f61d80-bc0c-11eb-8e79-429c9d1def9d.png)

#### Kendala
Saat ujicoba program awal, ada masalah saat menjalankan opsi \*. Hal ini dikarenakan pada saat menggunakan argument \* pada bash, \* akan dianggap sebuah auto-completion pada saat dipanggil sehingga tidak akan masuk pada program .c. Untuk menyelesaikannya kita menambahkan else dan menganggap argument tersebut sebagai sebuah file.
#### Hasil Running Program
# Referensi
## 1
1. https://www.geeksforgeeks.org/socket-programming-cc
2. https://www.binarytides.com/socket-programming-c-linux-tutorial/

## 2
1. https://www.programiz.com/c-programming/examples/matrix-multiplication
2. https://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell
3. https://gist.github.com/mplewis/5279108
4. https://www.geeksforgeeks.org/multiplication-of-matrix-using-threads/
5. https://github.com/Daemshad/Matrix-Multiply-Using-Threads/blob/master/main.c

## 3
1. https://stackoverflow.com/questions/1352749/multiple-arguments-to-function-called-by-pthread-create
