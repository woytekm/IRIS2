
#define _BSD_SOURCE
#define _POSIX_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

struct global {
	pid_t child;
	char *argv0;
} global;

void monitor(pid_t pid, float duration, int forked_for_ya,int verbose);
void prepare(void);
void usage(char *prog);
void linger(float f,int syscall,int verbose);

void usage(char *prog)
	{
	fprintf(stderr,"Usage: %s [-v] duration [pid|command]\n", prog);
	fprintf(stderr,"-v means to run verbosely\n");
	fprintf(stderr,"pid is the process id of a preexisting process to slow down\n");
	fprintf(stderr,"command is a command that slowdown should initiate itself and then slowdown\n");
	exit(1);
	}

void caught_signal(int signo)
	{
	kill(global.child,SIGCONT);
	fprintf(stderr,"%s: exiting on signal %d\n", global.argv0, signo);
	exit(0);
	}

void linger(float f,int syscall,int verbose)
	{
	static long iterations = 0;
	int seconds;
#if defined(__linux)
	__useconds_t millionths;
#endif
#if defined(__sun)
	useconds_t millionths;
#endif
	seconds = (int) f;
	millionths = (f - seconds) * 1000000;
	if (verbose)
		{
		fprintf(stderr,"pausing %f on syscall %d, %ld iterations\n",
			f,syscall,iterations);
		}
	sleep(seconds);
	usleep(millionths);
	iterations++;
	}

int main(int argc,char *argv[])
	{   
   pid_t child;
	char **arg_vec=NULL;
	float duration;
	int argno = 1;
	int verbose = 0;
	if (argc >= 3)
		{
		if (strcmp(argv[argno],"-v") == 0 && argc >= 4)
			{
			if (argc >= 4)
				{
				verbose = 1;
				argno++;
				}
			else
				{
				usage(argv[0]);
				}
			}
		sscanf(argv[argno++],"%f",&duration);
		if (sscanf(argv[argno],"%d",(int *)&child) != 1)
			{
			child = 0;
			}
		if (child == 0)
			{
			int i,len;
#if NEVER
			char *p;
#endif
			/* skip two: the prog name, and the duration.  And then we need
			 * an extra one for the new prog name: 1+1-1 is 1.
			 */
			len = argc - 1;
			arg_vec = malloc(sizeof(char *)*len);
			if (arg_vec == NULL)
				{
				fprintf(stderr,"malloc failed\n");
				exit(1);
				}
			for (i=0; i<len-1; i++)
				{
				arg_vec[i] = argv[i+argno];
				}
			arg_vec[len-1] = NULL;
			if (verbose)
				{
				fprintf(stderr,"Attempting to monitor new prog %s\n",arg_vec[0]);
				}
#if NEVER
			for (p=&(arg_vec[0]); p != NULL; p++)
				{
				fprintf(stderr,"arg_vec[%d] is %s\n", p - &(arg_vec[0]), p);
				}
#endif
			for (i=0; i < len; i++)
				{
				if (arg_vec[i] == NULL)
					{
					if (verbose)
						{
						fprintf(stderr,"arg_vec[%d] is %s\n", i, "NULL");
						}
					}
				else
					{
					if (verbose)
						{
						fprintf(stderr,"arg_vec[%d] is %s\n", i, arg_vec[i]);
						}
					}
				}
			}
		else
			{
			if (verbose)
				{
				fprintf(stderr,"Attempting to monitor preexisting pid %d\n",
					(int)child);
				}
			}
		}
	else
		{
		usage(argv[0]);
		}
	/* for use by caught_signal */
	global.child = child;
	global.argv0 = argv[0];
	signal(SIGTERM,caught_signal);
	signal(SIGINT,caught_signal);
	if (arg_vec == NULL)
		{
		monitor(child,duration,0,verbose);
		}
	else
		{
		child = fork();
		if (child == 0)
			{
			prepare();
			if (verbose)
				{
				fprintf(stderr,"Exec'ing %s in 2 seconds\n",arg_vec[0]);
				}
			/* ugly race window */
			sleep(2);
			execvp(arg_vec[0], &(arg_vec[0]));
			perror("execl failed");
			exit(1);
			}
		else
			{
			monitor(child,duration,1,verbose);
			}
		}
   return 0;
   }

