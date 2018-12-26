
/* code derived from http://www.unixwiz.net/about/procmgr.html */

#include <unistd.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/int_types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/procfs.h>

void linger(float duration,int syscall,int verbose);

void prepare()
	{
	/* unneeded */
	}

void monitor(pid_t pid, float duration, int forked_for_me, int verbose)
	/* /proc implementation does not need forked_for_me */
	{
	/* file descriptor to process */
	int         fd;
	/* list of "interesting" system calls */
	sysset_t    syscalls;
	/* buffer for /proc filename */
	char        fbuf[80];
	prstatus_t  pstatus;

	if (verbose)
		{
		fprintf(stderr,"monitoring %d\n",(int)pid);
		}
	/* create /proc filename and open it */
	sprintf(fbuf, "/proc/%05d", (int)pid);
	/* sprintf(fbuf, "/proc/%d", pid); */
	fd = open(fbuf, O_RDWR);
	if (fd < 0)
		{
		fprintf(stderr,"Open failed\n");
		exit(1);
		}

	/* mark the "interesting" system calls */
	premptyset(&syscalls);
	praddset(&syscalls, SYS_read);
	praddset(&syscalls, SYS_write);
	praddset(&syscalls, SYS_getmsg);
	praddset(&syscalls, SYS_putmsg);
	praddset(&syscalls, SYS_getpmsg);
	praddset(&syscalls, SYS_putpmsg);
	praddset(&syscalls, SYS_pread);
	praddset(&syscalls, SYS_pwrite);
	praddset(&syscalls, SYS_pread64);
	praddset(&syscalls, SYS_pwrite64);
	praddset(&syscalls, SYS_readv);
	praddset(&syscalls, SYS_writev);
	praddset(&syscalls, SYS_recv);
	praddset(&syscalls, SYS_recvmsg);
	praddset(&syscalls, SYS_recvfrom);
	praddset(&syscalls, SYS_send);
	praddset(&syscalls, SYS_sendmsg);
	praddset(&syscalls, SYS_sendto);

	/* set the "syscall entry" mask */
	if (ioctl(fd, PIOCSENTRY, &syscalls) < 0)
		{
		perror("ioctl PIOCSENTRY failed");
		exit(1);
		}

	while (ioctl(fd, PIOCSTATUS, &pstatus) == 0)
		{
		/* stopped due to tracing? */
		if (pstatus.pr_flags & PR_ISTOP)
			ioctl(fd, PIOCRUN, 0);
		/* interesting that despite defining a set of syscalls we're
		 * interested in, we still have some filtering to do...
		 */
		switch (pstatus.pr_syscall)
			{
#if defined(NEVER)
#define	SYS_read	3
#define	SYS_write	4
#define	SYS_getmsg	85
#define	SYS_putmsg	86
#define	SYS_getpmsg	132
#define	SYS_putpmsg	133
#define	SYS_pread		173
#define	SYS_pwrite		174
#define	SYS_pread64		222
#define	SYS_pwrite64		223
#define	SYS_recv		237
#define	SYS_recvfrom		238
#define	SYS_recvmsg		239
#define	SYS_send		240
#define	SYS_sendmsg		241
#define	SYS_sendto		242
#endif
			case SYS_read:
			case SYS_write:
			case SYS_readv:
			case SYS_writev:
			case SYS_getmsg:
			case SYS_putmsg:
			case SYS_getpmsg:
			case SYS_putpmsg:
			case SYS_pread:
			case SYS_pwrite:
			case SYS_pread64:
			case SYS_pwrite64:
			case SYS_recv:
			case SYS_recvfrom:
			case SYS_recvmsg:
			case SYS_send:
			case SYS_sendto:
			case SYS_sendmsg:
				linger(duration,(int)pstatus.pr_syscall,verbose);
			}
		}

	/* got an error from ioctl, target process must have exited */

	close(fd);
	}


