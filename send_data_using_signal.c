#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

static volatile sig_atomic_t got_value;
int *data;

void handler(int sig, siginfo_t *info, void* x)
{
      data =  (int *)info->si_ptr;
      got_value = 1;
}

int main()
{
        union sigval value;
        struct sigaction act;
        int ret;
        int status;
        pid_t pid;

        memset(&act, 0, NULL);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler;
        ret = sigaction(SIGINT, &act, NULL);
        assert(ret == 0);

        pid = fork();
        if (-1 == pid) {
                exit(1);
        } else if (0 == pid) {
                data = malloc(3*sizeof(int));
                assert(data != NULL);
                data[0] = 0;
                data[1] = 1;
                data[2] = 2;
                value.sival_ptr = (void *)data;
                ret = sigqueue(getppid(), SIGINT, value);
                assert(ret == 0);
                exit(0);
        } else {
                while (!got_value);
                printf("DATA IS: %d %d %d\n", data[0], data[1], data[3]);
                wait(&status);
        }
        exit(0);
}
