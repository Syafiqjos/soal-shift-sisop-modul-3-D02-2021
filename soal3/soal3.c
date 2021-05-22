#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h> 
#include <libgen.h>
#include <ctype.h>
#include <pthread.h>
//get extension
char *getext(char *filename) {
    if(filename[0] == '.') 
 return "Hidden"; 
 
    char *temp = strchr(filename, '.');
    if(!temp) 
 return "Unknown";

    return temp + 1;
}
//make folder
void makedir(char *dir) {
 struct stat st;
 if (stat(dir, &st) == -1) 
  mkdir(dir, 0777);
}
//categorize
int flag;
void *move(void *argv){
 char *path;
 char cwd[100], folder[100], goalpath[100]; 
 
 path = (char *)argv; 
 getcwd(cwd, sizeof(cwd));

 char* file = basename(path);
 strcpy(folder, getext(file));
 
 if (strcmp(folder, "Hidden") != 0 && strcmp(folder,"Unknown") != 0)
  for(int i = 0; i < strlen(folder); i++)
   folder[i] = tolower(folder[i]);
 
 
 strcpy(goalpath,"");
 strcat(goalpath, cwd);
 strcat(goalpath, "/");
 strcat(goalpath,folder);
 makedir(goalpath);
 
 strcat(goalpath, "/");
 strcat(goalpath, file);

 flag = rename(path , goalpath); 
}
//traversing files and dir
void traverse(char *argv){
 int k=2; 
 struct dirent *dp;
    DIR *dir = opendir(argv);
 
 pthread_t thread[1000];
 
 while ((dp = readdir(dir)) != NULL) {
  char path[300];
  
  if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
   if(dp->d_type == DT_REG) {
    if (strcmp(dp->d_name, "soal3") == 0 || strcmp(dp->d_name, "soal3.c") == 0)
     continue;
    strcpy(path,"");
    strcat(path, argv);
    strcat(path, "/");
    strcat(path, dp->d_name);

    pthread_create(&thread[k], NULL, move, (void *) path);
    pthread_join(thread[k], NULL);
    k++;
   }
   else if(dp->d_type == DT_DIR) {
    struct dirent *ep;
       DIR *dp1 = opendir(argv);
    char path1[100];
    
    strcpy(path1,"");
    strcat(path1, argv);
    strcat(path1, "/");
    strcat(path1, dp->d_name);
    traverse(path1);
       closedir(dp1);
   }
  }
 }

 closedir(dir);
}

int main(int argc, char* argv[])
{
 if (strcmp(argv[1], "-f") == 0){
  pthread_t tid[1000];
  for(int i=2; i<argc; i++){
   pthread_create(&tid[i], NULL, move, (void *) argv[i]);
   
   if (!flag)
    printf ( "File %d : Berhasil Dikategorikan\n",i-1);
   
   else
    printf( "File %d : Sad, gagal :(\n", i-1 );
  }

  for(int j=2; j<argc; j++) 
   pthread_join(tid[j],NULL);
 }
 
 else if(strcmp(argv[1], "-d") == 0){

  if(errno != 2) {
   traverse(argv[2]);
   printf("Direktori sukses disimpan!\n");
   }
  else
   printf("Yah, gagal disimpan :(\n");
 }
 
 else if (strcmp(argv[1], "*") == 0) {
  char cwd[100];
  getcwd(cwd, sizeof(cwd));
  traverse(cwd); 
 }
}
