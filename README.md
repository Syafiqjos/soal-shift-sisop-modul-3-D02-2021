# soal-shift-sisop-modul-3-D02-2021
- Jessica Tasyanita (05111940000043)
- Ahmad Syafiq Aqil Wafi (05111940000089)
- Maximilian H M Lingga (05111940000092)

## Soal 1
### Tujuan
### 1A. (isi maksud soal)
#### Source Code
#### Cara pengerjaan 
#### Kendala

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
