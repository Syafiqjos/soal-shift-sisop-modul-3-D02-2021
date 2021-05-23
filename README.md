# soal-shift-sisop-modul-3-D02-2021
- Jessica Tasyanita (05111940000043)
- Ahmad Syafiq Aqil Wafi (05111940000089)
- Maximilian H M Lingga (05111940000092)

## Soal 1
### Tujuan
Membuat suatu system server dan client menggunakan socket programming yang dapat menerima multi-connection untuk melakukan tugas penyimpanan database buku.

### 1A. Membuat system server dan client dengan socket programming yang dapat menerima multi-connection 

#### Client
#### Source Code
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

#### Server
#### Source Code
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

#### Kendala
- Untuk setiap pengiriman file dan pengiriman pesan yang terjadi pada server maupun client diberikan timout sebesar 1 - 2 detik. Hal ini dilakukan untuk meminimalisasikan error kebocoran data yang dapat terjadi. Terdapat suatu hal yang membuat data yang dikirim sama dengan pesan yang dikirim jika tidak diberikan timeout atau jeda.
- Untuk melakukan multi-connection hal yang perlu dilakukan adalah melakukan listen ulang pada client yang baru, bukan melakukan bind ulang. Karena jika socket melakukan bind ulang terdapat error berupa Port / Address is in use meskipun pada proses yang sama.

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
3. Menjalankan opsi -d
```
else if(strcmp(argv[1], "-d") == 0){
		mode = 1;
		if(errno != 2) {
			rec(argv[2]);
			printf("Direktori sukses disimpan!\n");
		}
		else {
		     	printf("Yah, gagal disimpan :(\n");
		}
	}
```
4.Menjalankan opsi *
```
else if (strcmp(argv[1], "*") == 0) {
		char cwd[128];
		getcwd(cwd, sizeof(cwd));
		rec(cwd);
	}
}
```
#### Kendala
Saat ujicoba program awal, ada masalah saat menjalankan opsi * dan diperbaiki setelah itu.
#### Hasil Running Program
# Referensi
## 1
## 2
1. https://www.programiz.com/c-programming/examples/matrix-multiplication
2. https://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell
3. https://gist.github.com/mplewis/5279108
4. https://www.geeksforgeeks.org/multiplication-of-matrix-using-threads/
5. https://github.com/Daemshad/Matrix-Multiply-Using-Threads/blob/master/main.c
## 3
