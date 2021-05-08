#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int pid;
int pipe1[2];
int pipe2[2];


void exec1() {
  dup2(pipe1[1], 1);

  close(pipe1[0]);
  close(pipe1[1]);

  execlp("ps", "ps", "aux", NULL);
  // exec gajalan, keluar
  perror("bad exec ps");
  _exit(1);
}

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

void exec3() {
  dup2(pipe2[0], 0);

  close(pipe2[0]);
  close(pipe2[1]);

  execlp("head", "head", "-5", NULL);

  perror("bad exec head");
  _exit(1);


  exit(0);
}

int main() {

  // bikin pipe1
  if (pipe(pipe1) == -1) {
    perror("bad pipe1");
    exit(1);
  }

  if ((pid = fork()) == -1) {
    perror("bad fork1");
    exit(1);
  } else if (pid == 0) {
    exec1();
  }

  if (pipe(pipe2) == -1) {
    perror("bad pipe2");
    exit(1);
  }

  if ((pid = fork()) == -1) {
    perror("bad fork2");
    exit(1);
  } else if (pid == 0) {
    exec2();
  }

  close(pipe1[0]);
  close(pipe1[1]);

  if ((pid = fork()) == -1) {
    perror("bad fork3");
    exit(1);
  } else if (pid == 0) {
    exec3();
    //exit(0);
  }

}
