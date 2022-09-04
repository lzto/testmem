/*
 * THIS PROGRAM IS DISTRIBUTED UNDER GPLv2
 * test memory speed
 * 2013-2022 Tong Zhang<ztong0001@gmail.com>
 */
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

typedef void (*sighandler_t)(int);

size_t memsize;
struct timeval time_r_start;
struct timeval time_r_end;
struct timeval time_w_start;
struct timeval time_w_end;

double r_peak_perf;
double r_average_perf;
double w_peak_perf;
double w_average_perf;

unsigned long cycle_all;
unsigned long cycle_inf_w;
unsigned long cycle_inf_r;

char silent;

int work_mem(char);

void signal_int(int signo) {
  printf("\nCaught signal %d\n", signo);
  printf("--------------------------------------\n");
  printf("Peak\tW\tR\n");
  printf("    \t%fMB/s\t%fMB/s\n", w_peak_perf, r_peak_perf);
  printf("Average\tW\tR\n");
  printf("       \t%fMB/s\t%fMB/s\n", w_average_perf, r_average_perf);
  printf("Test complete in %ld cycles\n", cycle_all);
  printf("with write burst inf %ld cycles, %f Percent\n", cycle_inf_w,
         (double)cycle_inf_w / (double)cycle_all * 100.0);
  printf("with read  burst inf %ld cycles, %f Percent\n", cycle_inf_r,
         (double)cycle_inf_r / (double)cycle_all * 100.0);
  printf("--------------------------------------\n");
  exit(0);
}

void usage(const char *bin) {
  printf("%s [auto|size] [s]\n", bin);
  printf(" auto - automatic increase array size from 0 to inf\n"
         " size - array size in byte\n"
         " s - only print final result\n");
  exit(1);
}

inline int numbergen() {
  // srandom(time(NULL));
  // return (int)random();
  return 0;
}

int work_freemem() {
  memsize = 1;
  while (1) {
    cycle_all++;
    printf("\nmemsize : %zu\n", memsize);
    if (work_mem(numbergen()) == 0) {
      memsize *= 2;
    } else {
      printf("failed @ %zu \n"
             "revert to liner increasment\n",
             memsize);
      memsize /= 2;
      break;
    }
  }
  while (1) {
    cycle_all++;
    if (work_mem(numbergen()) == 0) {
      memsize += 1024;
    } else {
      memsize -= 1024;
    }
    printf("\nmemsize : %zu byte\n", memsize);
  }
  return 0;
}

///
/// val -- some value to fill the array
///
inline int work_mem(char val) {
  char dstbuffer[128];
  size_t i;
  memset(dstbuffer, 0, 128);
  char *buffer = (char *)malloc(memsize);
  if (buffer == NULL)
    return -1;
  // touch the buffer make sure no lazy initialization
  for (i = 0; i < memsize; i++)
    buffer[i] = 0;
  // write test
  gettimeofday(&time_w_start, NULL);
  for (i = 0; i < memsize; i++)
    buffer[i] = val;
  gettimeofday(&time_w_end, NULL);
  double wspeed = ((double)memsize / (1024.0 * 1024.0)) /
                  ((double)(time_w_end.tv_sec - time_w_start.tv_sec) +
                   (time_w_end.tv_usec - time_w_start.tv_usec) / 1000000.0);

  if (wspeed != INFINITY) {
    if (wspeed > w_peak_perf)
      w_peak_perf = wspeed;
    w_average_perf = (w_average_perf + wspeed) / 2.0;
  } else {
    cycle_inf_w++;
  }
  if (!silent)
    printf("\rw = %f MB/s", wspeed);

  // read test
  gettimeofday(&time_r_start, NULL);
  for (i = 0; i < memsize; i++)
    if (unlikely(buffer[i] != val))
      goto fail;
  gettimeofday(&time_r_end, NULL);

  double rspeed = ((double)memsize / (1024.0 * 1024.0)) /
                  ((double)(time_r_end.tv_sec - time_r_start.tv_sec) +
                   (time_r_end.tv_usec - time_r_start.tv_usec) / 1000000.0);
  if (rspeed != INFINITY) {
    if (rspeed > r_peak_perf)
      r_peak_perf = rspeed;
    r_average_perf = (r_average_perf + rspeed) / 2.0;
  } else {
    cycle_inf_r++;
  }
  if (!silent)
    printf(" r = %f MB/s", rspeed);

  free(buffer);
  return 0;
fail:
  free(buffer);
  return -1;
}

int main(int argc, char **argv) {
  if (argc < 2)
    usage(argv[0]);
  signal(SIGINT, signal_int);
  memsize = atoi(argv[1]);
  if (argc >= 3) {
    if (strcmp("s", argv[2]) == 0) {
      silent = 1;
    }
  }

  if (strcmp("auto", argv[1]) == 0) {
    memsize = 1;
    printf("testing mem auto size\n");
    work_freemem();
  } else if (memsize != 0) {
    printf("testing mem size: %zu Byte\n", memsize);
    int x = 0;
    while (1) {
      cycle_all++;
      if (work_mem(x++)) {
        fprintf(stderr, "fatal error\n");
        exit(-1);
      }
    }
  }
  usage(argv[0]);
  return 1;
}
