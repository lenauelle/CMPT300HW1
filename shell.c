#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <limits.h>

#define min(a, b) (((a) > (b)) ? (b) : (a))

extern char **get_line();

void execblock(char *cmd, char *argv[]) {
  int status;
  
  pid_t pid = fork();
  switch(pid) {
    case -1: {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    case 0: {
      execvp(cmd, argv);
      _exit(EXIT_SUCCESS);
      break;
    }
    default: {
      waitpid(pid, &status, 0);
    }
  }
}

void execbg(char *cmd, char *argv[]) {
  struct sigaction sa;
  int i, fd, fdtablesize;
  pid_t pid;

  umask(0);
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTTOU, &sa, NULL);
  sigaction(SIGTTIN, &sa, NULL);
  sigaction(SIGTSTP, &sa, NULL);
  sigaction(SIGHUP, &sa, NULL);

  pid = fork();
  if(pid == -1) {
    perror("fork");  
    exit(EXIT_FAILURE);
  }
  else if(pid == 0) {
    setpgrp();
    execvp(cmd, argv);
    _exit(EXIT_SUCCESS);
  }
  sigaction(SIGCHLD, &sa, NULL);
}

void execmode(char *cmd, char *argv[], int mode) {
  switch(mode) {
    case 0: {
      execblock(cmd, argv);
    break;
    }
    case 1: {
      execbg(cmd, argv);
    }
  }
}

/*
//Function: commapipe()
//used to implement pipe command; not in use at the moment

void commapipe(char *s1[], char *s2[]) {
  int fd[2];
  pipe(fd);
  pid_t pid = fork();
  if(pid == 0) {
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    execmode(s1[0], s1, 0);
    _exit(EXIT_SUCCESS);
  }
  else {
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    execmode(s2[0], s2, 0);
  }
}
*/

void main() {
  setpgrp();

  char **args; 
  int count, minop, i, errflag;
  int opset[3]; //mark the commands of |, > and <
  time_t timep;

  struct tm *p;

  //current time
  time(&timep);
  printf("%s", asctime(localtime(&timep)));

  while(1) {
    errflag = 0;
    opset[0] = -1;
    opset[1] = -1;
    opset[2] = -1;   

    args = get_line();

    for(count = 0; args[count] != NULL; count++) {
      if(!strcmp(args[count], "|")) {
        if(opset[0] != -1) {
          printf("More than one '|'. Not supported.\n");
          errflag = 1;
        }
        else {opset[0] = count;}
      }
      else if(!strcmp(args[count], ">")) {
        if(opset[1] != -1) {
          printf("More than one '>'. Not supported.\n");
          errflag = 1;
        }
        else {opset[1] = count;}
      }
      else if(!strcmp(args[count], "<")) {
        if(opset[2] != -1) {
          printf("More than one '<'. Not supported.\n");
          errflag = 1;
        }
        else {opset[2] = count;}
      }
    }
 
    if(errflag == 1) continue; //Not allowing inputs like "ls < a < b"

    if(count == 1 && !strcmp(args[0], "exit")) {
      exit(EXIT_SUCCESS);
    }

    if(opset[0] == -1 && opset[1] == -1 && opset[2] == -1) {
      //4. bg execution
      if(!strcmp(args[count - 1], "&")) {
        execmode(args[0], args, 1);
      }
      else {
      //2.3. fg execution
        execmode(args[0], args, 0);
      }
    }
    else if(opset[0] != -1) {
      /*
      //7. pipe

      //Error: Core dumped
      //Not able to handle. I give up :(

      int i;
      char *temp1[opset[0] + 1];
      char *temp2[count - opset[0]];
      for(i = 0; i < opset[0] + 1; i++) {
        temp1[i] = malloc(strlen(args[i] + 1) * sizeof(char));
        strcpy(temp1[i], args[i]);
      }
      temp1[i - 1] = NULL;
      for(i = 0; i < count - opset[0] - 1; i++) {
        temp2[i] = malloc(strlen(args[opset[0] + i + 1] + 1) * sizeof(char));
        strcpy(temp2[i], args[opset[0] + i + 1]);
      }
      temp2[i] = malloc(sizeof(char));
      temp2[i] = NULL;
      commapipe(temp1, temp2);
    */
      printf("Sorry, pipe command is not yet implemented :(\n");
    }
    else if(opset[2] != -1 && opset[1] == -1) {
      //6. redirecting input 
      //input only, that is, only needs the filename
      minop = opset[2];
      char *temp[minop + 2];

      for(i = 0; i < minop + 1; i++) {
        temp[i] = malloc(strlen(args[i] + 1) * sizeof(char));
        strcpy(temp[i], args[i]);
      }
      temp[i] = malloc(strlen(args[i] + 1) * sizeof(char));
      strcpy(temp[i - 1], args[opset[2] + 1]);
      temp[i] = NULL;
      execmode(args[0], temp, 0);
    }
    else if(opset[1] != -1) {
      //5.6. redirecting output/input 
      if(opset[2] != -1) {
        minop = min(opset[1], opset[2]);
      }
      else {
        minop = opset[1];
      }

      char *temp[minop + 1];
      for(i = 0; i < minop + 1; i++) {
        temp[i] = malloc(strlen(args[i] + 1) * sizeof(char));
        strcpy(temp[i], args[i]);
      }
      temp[i - 1] = NULL;

      if(opset[2] != -1) {
        //input and output
        if(freopen(args[opset[2] + 1], "r", stdin) == NULL) {
          fprintf(stderr, "error redirecting stdin\n");
        }
        if(freopen(args[opset[1] + 1], "w", stdout) == NULL) {
          fprintf(stderr, "error redirecting stdout\n");
        }
        execmode(temp[0], temp, 0);  
        fclose(stdin);
        fclose(stdout);
        freopen("/dev/tty", "r", stdin);
        freopen("/dev/tty", "w", stdout);
      }
      else {
        //output only
        if(freopen(args[opset[1] + 1], "w", stdout) == NULL) {
          fprintf(stderr, "error redirecting stdout\n");
        }
        execmode(temp[0], temp, 0);  
        fclose(stdout);
        freopen("/dev/tty", "w", stdout);
      }
    }
  }
}

