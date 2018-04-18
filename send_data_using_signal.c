#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <assert.h>

static volatile sig_atomic_t got_value;
struct data {
	int a, b, c;
};

struct data cdata;

void handler1(int sig, siginfo_t *info, void* x)
{
	cdata.a = info->si_int;
      ++got_value;
}

void handler2(int sig, siginfo_t *info, void* x)
{
	cdata.b = info->si_int;
      ++got_value;
}

void handler3(int sig, siginfo_t *info, void* x)
{
	cdata.c = info->si_int;
      ++got_value;
}

int main()
{
        struct sigaction act;
        int ret;
        int status;
        pid_t pid;

        memset(&act, 0, sizeof(struct sigaction));
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler1;
        ret = sigaction(SIGINT, &act, NULL);
        assert(ret == 0);

        act.sa_sigaction = handler2;
        ret = sigaction(SIGUSR1, &act, NULL);
        assert(ret == 0);

        act.sa_sigaction = handler3;
        ret = sigaction(SIGUSR2, &act, NULL);
        assert(ret == 0);

        pid = fork();
        if (-1 == pid) {
                exit(1);
        } else if (0 == pid) {
		siginfo_t uinfo;
		uinfo.si_code = SI_QUEUE;
		uinfo.si_pid = getpid();
		uinfo.si_int = 1;
		ret = syscall(SYS_rt_sigqueueinfo, getppid(), SIGINT, &uinfo);
                assert(ret == 0);
		uinfo.si_int = 2;
		ret = syscall(SYS_rt_sigqueueinfo, getppid(), SIGUSR1, &uinfo);
                assert(ret == 0);
		uinfo.si_int = 3;
		ret = syscall(SYS_rt_sigqueueinfo, getppid(), SIGUSR2, &uinfo);
                assert(ret == 0);
                exit(0);
        } else {
                while (3 != got_value);
                printf("DATA IS: %d %d %d\n", cdata.a, cdata.b, cdata.c);
                wait(&status);
        }
        exit(0);
}
