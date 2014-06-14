/*
 * THIS PROGRAM IS DISTRIBUTED UNDER GPLv2
 * testmem
 * 2013-2014 by lovewilliam <lovewilliam@gmail.com>
 */
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

typedef void(*sighandler_t)(int);

static unsigned int memsize;
static struct timeval time_r_start;
static struct timeval time_r_end;
static struct timeval time_w_start;
static struct timeval time_w_end;

static double r_peak_perf;
static double r_average_perf;
static double w_peak_perf;
static double w_average_perf;

static unsigned long cycle_all;
static unsigned long cycle_inf_w;
static unsigned long cycle_inf_r;

static char silent;

int work_mem(int val);

void signal_int()
{
	printf("\nCaught SIGNAL\n");
	printf("--------------------------------------\n");
	printf("Peak\tW\tR\n");
	printf("    \t%fMB/s\t%fMB/s\n",w_peak_perf,r_peak_perf);
	printf("Average\tW\tR\n");
	printf("       \t%fMB/s\t%fMB/s\n",w_average_perf,r_average_perf);
	printf("Test complete in %ld cycles\n",cycle_all);
	printf("with write burst inf %ld cycles, %f Percent\n",cycle_inf_w,(double)cycle_inf_w/(double)cycle_all*100.0);
	printf("with read  burst inf %ld cycles, %f Percent\n",cycle_inf_r,(double)cycle_inf_r/(double)cycle_all*100.0);
	printf("--------------------------------------\n");
	exit(0);
}

void usage(char* appname)
{
	fprintf(stderr,"%s [auto]|[mem_size in %lu byte] [s]\n",appname,sizeof(int));
}

inline int numbergen()
{
	//srandom(time(NULL));
	//return (int)random();
	return 0;
}

int work_freemem()
{
	memsize = 1;
	while(1)
	{
		cycle_all++;
		printf("\nmemsize : %d byte\n",memsize);
		if(work_mem(numbergen())==0)
		{
			memsize*=2;
		}else
		{
			printf("failed at %d byte\n"
				"revert to liner increasment\n",memsize);
			memsize/=2;
			break;
		}
	}
	while(1)
	{
		cycle_all++;
		if(work_mem(numbergen())==0)
		{
			memsize+=1024;
		}else
		{
			memsize-=1024;
		}
		printf("\nmemsize : %d byte\n",memsize);
	}
	return 0;
}

inline int work_mem(int val)
{
	double speed;
	int* buffer = malloc(memsize*sizeof(int));
	if(buffer==NULL)
	{
		return -1;
	}
	int i;
	gettimeofday(&time_w_start,NULL);
	for(i = 0;i<memsize;i++)
	{
		buffer[i] = val;
	}

	gettimeofday(&time_w_end,NULL);

	speed = ((double)memsize/(1024.0*1024.0))/
				((double)(time_w_end.tv_sec - time_w_start.tv_sec)+
				 (time_w_end.tv_usec-time_w_start.tv_usec)/1000000.0);

	if(speed!=INFINITY)
	{
		if(speed>w_peak_perf)
		{
			w_peak_perf = speed;
		}
		w_average_perf = ( w_average_perf + speed )/2.0;
	}else
	{
		cycle_inf_w++;
	}
	if(!silent)
		printf("\rw = %f MB/s",speed);

	//sleep(5);
	gettimeofday(&time_r_start,NULL);
	for(i = 0;i<memsize;i++)
	{
		if(buffer[i]!=val)
		{
			fprintf(stderr,"FATAL at %d\n",i);
			free(buffer);
			return -1;
		}
	}
	gettimeofday(&time_r_end,NULL);

	speed = ((double)memsize/(1024.0*1024.0))/
			((double)(time_r_end.tv_sec - time_r_start.tv_sec)+
			 (time_r_end.tv_usec-time_r_start.tv_usec)/1000000.0);
	if(speed!=INFINITY)
	{
		if(speed>r_peak_perf)
		{
			r_peak_perf = speed;
		}
		r_average_perf = ( r_average_perf + speed )/2.0;
	}else
	{
		cycle_inf_r++;
	}
	if(!silent)
		printf(" r = %f MB/s",speed);

	free(buffer);
	return 0;
}

int main(int argc,char** argv)
{
	int x = 0;
	if(argc<2)
	{
		usage(argv[0]);
		return 0;
	}
	signal(SIGINT,(sighandler_t)signal_int);
	memsize = atoi(argv[1]);
	if(argc>=3)
	{
		if(strcmp("s",argv[2])==0)
		{
			silent = 1;
		}
	}

	if(strcmp("auto",argv[1])==0)
	{
		memsize = 1;
		printf("testing mem auto size\n");
		work_freemem();
	}else if(memsize!=0)
	{
		printf("testing mem size: %lu Byte\n",memsize*sizeof(int));
		while(1)
		{
			cycle_all++;
			if(work_mem(x++)==0)
			{
				//fprintf(stdout,"mem test passed\n");
			}else
			{
				fprintf(stderr,"fatal\n");
				exit(-1);
			}
	//		sleep(1);
		}
	}
	usage(argv[0]);
	return 0;
}

