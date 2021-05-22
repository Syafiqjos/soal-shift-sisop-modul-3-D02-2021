#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h> 
#include <ctype.h>
#include <pthread.h>
#include <libgen.h>
#include <errno.h>
#include <time.h>

typedef struct {
	char *path;
	int inde;
} rec_args;

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

//make folder
void make_directory (char *dir) {
	struct stat st;
	if (stat(dir, &st) == -1) {
		mkdir(dir, 0777);
	}
}

void *move_file(void *argv){
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

	sprintf(folderpath, "%s/%s", cwd, folder);

	make_directory(folderpath);

	sprintf(filepath, "%s/%s", folderpath, file);
 
	int failed = rename(path , folderpath); 
	
	if (!failed) {
		printf ( "File %d : Berhasil Dikategorikan\n",inde - 1);
	}
    	else {
	    	printf( "File %d : Sad, gagal :(\n", inde - 1 );
	}
}

//traversal to files and folder
void rec(char *argv){
	int k = 2; 
	struct dirent *dp;
	DIR *dir = opendir(argv);
 

       pthread_t thread[1000];
 

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

int main(int argc, char* argv[]) {
	if (argc == 0 || argc == 1){
		printf("Tidak ada argument -f atau -d ditemukan\n");
		return 0;
	}

	if (strcmp(argv[1], "-f") == 0){
		pthread_t tid[1000];
		for(int i = 2; i < argc; i++){
			
			rec_args *argg = malloc(sizeof(rec_args));
			argg->path = argv[i];

			pthread_create(&tid[i], NULL, move_file, (void *) argg); 
		}
	
		for(int j = 2; j < argc; j++)  {
			pthread_join(tid[j],NULL);
		}
	}
	
	else if(strcmp(argv[1], "-d") == 0){
		if(errno != 2) {
			rec(argv[2]);
			printf("Direktori sukses disimpan!\n");
		}
		else {
		     	printf("Yah, gagal disimpan :(\n");
		}
	}
	else if (strcmp(argv[1], "*") == 0) {
		char cwd[128];
		getcwd(cwd, sizeof(cwd));
		rec(cwd);
	}
}
