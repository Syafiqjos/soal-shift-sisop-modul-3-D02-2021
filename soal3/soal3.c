#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

void make_directory(char *path){
	pid_t child_id=fork ();
	if (child_id==0){
		printf("Program soal 3 terletak di"-> %s\n", path);
		execl("/bin/mkdir", 
	      	"/bin/mkdir",
	      	path,
	      	NULL);
	      	exit (0);
	}
	else {
		int status=0;
		waitpid (child_id,&status,0);
	}
}

void getname(char *arg, char *tipe){
	char string [1000];
	int text_file = file (string);
	while (tipe){
	if (tipe[strlen(tipe)-1] >= 'a' && tipe[strlen(tipe)-1] <= 'Z'){
	printf(tipe, "Berhasil dikategorikan");
	}
	else{
	printf ("Sad,gagal")
	}
	}
}

void move_files(char *path, char *category){
	pid_t child_id=fork ();
	if (child_id==0){
		printf("Moving Files -> %s -> %s\n", path,category);
		execl("/usr/bin/mv", 
	      	"/usr/bin/mv",
	      	path,category,
	      	NULL);
	      	exit (0);
	}
	else {
		int status=0;
		waitpid (child_id,&status,0);
	}
}
void make_category (char *path, char *directory){
	while (i=0;i<n;i++){
	printf("buat kategori" %s\n", path);
	move_files (path,directory);
	if (path = directory){
	printf("directori berhasil disimpan");
	}
	else{
	printf("Yah,gagal disimpan")
	}
	}
}

