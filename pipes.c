#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

int child(int fd) {
    FILE* stream = fdopen(fd, "r");
    char c;
    /* printf("child read from %p\n", (void*)stream); */
    while ((c = fgetc(stream)) != EOF) {
      putchar(c);
      putchar(' ');
    }
    fclose(stream);


    printf("child exit on c='%d' eof='%d'\n", c, EOF);
    printf("error: '%s'\n", strerror(errno));

    return EXIT_SUCCESS;
}

int parent(int fd) {
  FILE* file = fdopen(fd, "w");
  int ret = fprintf(file, "Hello luke, I am your father!\n");
  printf("wrote %d bytes\n", ret);
  fclose(file);

  return EXIT_SUCCESS;
}

int main() {
  int pipefd[2];
  if (pipe(pipefd)) {
    fprintf(stderr, "pipe failed\n");
    return 1;
  }
  printf("fd1 %d fd2 %d\n", pipefd[0], pipefd[1]);

  pid_t pid = fork();
  if (pid < (pid_t)0) { /* error */
    fprintf(stderr, "fork failed: %s\n", strerror(errno));
    return EXIT_SUCCESS;
  } else if (pid == 0) { /* child */
    /* close the pipe fd used in parent */
    if (close(pipefd[1])) {
      fprintf(stderr, "error closing fd2 %d\n", pipefd[1]);
    }
    return child(pipefd[0]);
  } else { /* parent */
    if (close(pipefd[0])) {
      fprintf(stderr, "error closing fd2 %d\n", pipefd[1]);
    }
    return parent(pipefd[1]);
  }
}
