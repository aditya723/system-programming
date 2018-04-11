#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t got_sigint;

pid_t parent;

void sigint_handler(int sig, siginfo_t *f, void *main_context) {
	
        ucontext_t *uc = main_context;
        if (parent == getpid() ) {
                sigaddset(&uc->uc_sigmask, SIGINT);
        }
    
        got_sigint = 1;
}

void create_child(void)
{

        pid_t pid = fork();
        sigset_t set;
        sigemptyset(&set);

        if (-1 == pid) {
                perror("Fork failed\n");
                exit(1);
        } else if (0 == pid) {
                parent = getpid();
                got_sigint = 0;
                sigprocmask(SIG_SETMASK, &set, NULL);
                printf("New Child\n Please send signal to create child\n");
                while (!got_sigint);
                create_child();
        }

}
int main(int argc, const char *argv[]) {

        struct sigaction act = {.sa_sigaction = sigint_handler, .sa_mask = 0, .sa_flags = SA_SIGINFO};
        sigset_t mask;
        int status;


        parent = getpid();

        sigaction(SIGINT, &act, NULL);
        printf("parent\n Please send signal to create child\n");

        while (!got_sigint);
        create_child();

        wait(&status);
        exit(0);
}
