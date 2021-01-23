#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

int child_main() {
  fprintf(stdout, "It cannot be!\n");
  fprintf(stdout, "My father is dead!\n");

  return EXIT_SUCCESS;
}

int child(int fd) {
  // dup2 makes the second file descriptor passed
  // point to the first file descriptor. so here we make
  // fd 1 (stdout) point to fd (our pipe to our parent)
  dup2(fd, 1);

  child_main();

  return EXIT_SUCCESS;
}

/* Tiny, in reality you'd use something like 1024 */
#define CHUNK_SIZE 16

int parent(int fd) {
  FILE* stream = fdopen(fd, "r");
  if (stream == NULL) {
    perror("failed to open file");
    return EXIT_FAILURE;
  }

  int size = CHUNK_SIZE;
  char* buf = 0;
  int seek = 0; /* position in buf */
  for (;;) {
    char* tmp = realloc(buf, sizeof(char) * (size + 1));
    if (tmp == NULL) {
      fprintf(stderr, "realloc of %lu bytes failed\n", sizeof(char)*size);
      return EXIT_FAILURE;
    }
    buf = tmp; /* OS might move memory, so we must use the possibly new pointer. */

    int n = fread(buf + (seek*sizeof(char)), 1, CHUNK_SIZE, stream);
    /* printf("read %d bytes\n", n); */

    seek += n;
    size += CHUNK_SIZE;
    if (n == 0) { break; }
  }
  /* I think this is required, but it was working before so no clue ._. */
  buf[seek] = '\0';

  printf("Read: '%s' from child\n", buf);

  if (fclose(stream) != 0) {
    perror("failed to close child stream reader");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int main() {
  int pipefd[2];
  if (pipe(pipefd)) {
    fprintf(stderr, "pipe failed\n");
    return 1;
  }
  printf("fd1 %d fd2 %d\n", pipefd[0], pipefd[1]);

  int read_fd = pipefd[0];
  int write_fd = pipefd[1];

  pid_t pid = fork();
  if (pid < (pid_t)0) { /* error */
    fprintf(stderr, "fork failed: %s\n", strerror(errno));
    return EXIT_SUCCESS;
  } else if (pid == 0) { /* child */
    /* close the pipe fd used in parent */
    if (close(read_fd) != 0) {
      perror("failed to close read_fd");
      return EXIT_FAILURE;
    }
    return child(write_fd);
  } else { /* parent */
    if (close(write_fd) != 0) {
      perror("failed to close write_fd");
      return EXIT_FAILURE;
    }

    return parent(read_fd);
  }
}
