#define _GNU_SOURCE
#include <fcntl.h>
#include <math.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void die(const char *message) {
  perror(message);
  exit(1);
}

struct run_args {
  struct timespec *start;
  char *const program;
  char *const input;
};

static int run_impl(void *a) {
  struct run_args *args = a;
  // Open the input file as STDIN.
  const int input = open(args->input, O_RDONLY);
  if (input == -1) die("open");
  const int new_stdin = dup2(input, STDIN_FILENO);
  if (new_stdin == -1) die("dup2");
  // Start the timer and execute the program.
  char *argv[] = {args->program, NULL};
  clock_gettime(CLOCK_MONOTONIC, args->start);
  execv(argv[0], argv);
  die("execv");
}
static char run_stack[4096];

// Runs the given program, passing the contents of the given input file to
// stdin, and returns the total execution time in microseconds.
static int run(char *program, char *input) {
  // We will call `clone` to start a new thread, and then use `execve` to start
  // the program from that thread. The timer is started from the new thread,
  // immediately before invoking `execve`, to avoid counting the thread
  // creation time.
  struct timespec start, end;
  struct run_args args = {
      .start = &start,  // The start time will be recorded by run_impl.
      .program = program,
      .input = input,
  };
  const int pid =
      clone(run_impl,
            run_stack + sizeof(run_stack), // Stack assumed to grow downwards.
            CLONE_VM | SIGCHLD, // Use the same memory map, signal on exit.
            &args);
  if (pid == -1) die("clone");
  int status;
  if (waitpid(pid, &status, 0) == -1) die("waitpid");
  clock_gettime(CLOCK_MONOTONIC, &end);
  if (status != 0) {
    fprintf(stderr, "program exited with code %d\n", status);
    exit(1);
  }
  const int sec = end.tv_sec - start.tv_sec;
  const int usec = (end.tv_nsec - start.tv_nsec) / 1000;
  return 1000000 * sec + usec;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: microtime <program> <input>\n");
    return 1;
  }
  char *const program = argv[1];
  char *const input = argv[2];
  // Run for 500ms.
  int runs = 0;
  int64_t sum = 0;
  int64_t sum_squares = 0;
  while (sum < 500000) {
    const int64_t time = run(program, input);
    sum += time;
    sum_squares += time * time;
    runs++;
  }
  const int64_t mean = sum / runs;
  const int64_t stddev = sqrt(sum_squares / runs - sum * sum / (runs * runs));
  fprintf(stderr, "Ran %d times, mean %dus, stddev %dus\n", runs, mean, stddev);
}
