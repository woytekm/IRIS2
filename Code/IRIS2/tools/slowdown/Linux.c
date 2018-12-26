
/* derived from an example at http://www.linuxjournal.com/article/6100 */

#define USE_BSD

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void linger(float duration,int syscall,int verbose);

void prepare()
	{
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
		perror("PTRACE_TRACEME failed");
		exit(1);
		}
	}

void monitor(pid_t child, float duration,int forked_for_me,int verbose)
	{
	long sys_call;
	int status,result;
	if (!forked_for_me)
		{
		/* this gives an "Operation not permitted error, even if we run it
		 * as root, but only if we forked our own process to monitor.  In
		 * the case that we're attaching to a process we didn't fork, then
		 * this is Required.
		 */
		if (ptrace(PTRACE_ATTACH,child,0,0) == -1)
			{
			perror("PTRACE_ATTACH failed");
			exit(1);
			}
		}
	for (;;)
		{
		wait(&status);
		if(WIFEXITED(status))
			{
			/* fprintf(stderr,"WIFEXITED\n"); */
			break;
			}
		/* fprintf(stderr,"In linux main loop\n"); */
#if NEVER
		/* just for prowling around a bit */
		for (int registerno=0; registerno<17; registerno++)
			{
			int value;
			value = ptrace(PTRACE_PEEKUSER, 
				child,
				4 * registerno,
				NULL);
			fprintf(stderr, "register number %d has value %d\n", registerno, value);
			}
#endif
		sys_call = ptrace(PTRACE_PEEKUSER, 
			child,
#if defined(linux)
#	if defined(ORIG_EAX)
			4 * ORIG_EAX, 
#	else
#		if defined(ORIG_RAX)
			4 * ORIG_RAX, 
#		endif
#	endif
#endif
			NULL);
		if (sys_call == -1)
			{
			if (errno == 0)
				{
				fprintf(stderr, "ptrace returned -1 but errno is 0, which should not happen - continuing anyway\n");
				}
			else
				{
				perror("PTRACE_PEEKUSER failed");
				exit(1);
				}
			}
		/* Useful way of checking on undelayed system calls */
		/* fprintf(stderr,"sys_call is %lx, SYS_pread64 is %x\n", sys_call,SYS_pread64); */
		switch (sys_call)
			{
			case SYS_write:
			case SYS_read:
#if defined(SYS_pwrite64)
			case SYS_pwrite64:
#endif
#if defined(SYS_pread64)
			case SYS_pread64:
#endif
			case SYS_writev:
			case SYS_readv:
			/* on linux, there is a socket system call, and then things
			 * like recv and send are subcalls.  Hence this is a bit
			 * broader than we really want, but it's probably OK anyway.
			 */
#if defined(linux)
#	if defined(SYS_socketcall)
			case SYS_socketcall:
#	else
			case SYS_sendmsg:
			case SYS_sendto:
			case SYS_recvfrom:
			case SYS_recvmsg:
#	endif
#endif
			/* case SYS_send: */
			/* case SYS_recv: */
				linger(duration,sys_call,verbose);
				result = ptrace(PTRACE_SYSCALL, child, NULL, NULL); 
				if (result == -1)
					{
					perror("PTRACE_SYSCALL failed");
					exit(1);
					}
				/* fprintf(stderr, "\n"); */
				break;
			default:
				result = ptrace(PTRACE_SYSCALL, child, NULL, NULL); 
				if (result == -1)
					{
					perror("PTRACE_SYSCALL failed");
					exit(1);
					}
				break;
			}
		}
	ptrace(PTRACE_SYSCALL, child, NULL, NULL);
   }

