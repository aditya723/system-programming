#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

static volatile sig_atomic_t got_signal;

void handler(int sig)
{
        printf("caught signal: %d\n", getpid());
        got_signal = 1;
}

int main() {

        pid_t child;
        pid_t children[3];
        int status;
        int i = 0;
        
        signal(SIGINT, handler);

        for (; i < 3; i++) {
                switch (child = fork()) {
                        case -1:
                                perror("could not create child: ");
                                break;
                        case 0:
                                printf("child: %d\n", getpid());
                                while (!got_signal);
                                _exit(0);
                        default:
                                children[i] = child;
                                setpgid(child, children[0]);
                }
        }
        sleep(1);
        /* send signal to all child */
        kill(-children[0], SIGINT);
        waitpid(children[0], &status, 0);
        waitpid(children[1], &status, 0);
        waitpid(children[2], &status, 0);
        exit(0);
}
