#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <sys/wait.h>

#define TIMER_FREQUENCY 1

void sigalrm_received(int sig)
{
        // this handler will be executed when program receives SIGALRM
}

void timer_expired(union sigval sig)
{
        // this function will be executed upon timer expiry
}

int main(void)
{
        struct sigaction act;
        clockid_t clock_id;
        timer_t timer_id;
        struct sigevent clock_sig_event;
        struct itimerspec timer_value;
        int ret;

        /* Register new action for SIGUSR1 */
        memset(&act, 0, sizeof(struct sigaction));
        act.sa_handler =  sigalrm_received;
        ret = sigaction(SIGALRM, &act, NULL);
        assert(ret == 0);

        /* Creating process interval timer */
        clock_id = CLOCK_MONOTONIC;
        memset(&clock_sig_event, 0, sizeof( struct sigevent));
        /*Read SIGEVENT(7) man page*/
        clock_sig_event.sigev_notify = SIGEV_THREAD;
        clock_sig_event.sigev_notify_function = timer_expired;
        clock_sig_event.sigev_notify_attributes = NULL;
        ret = timer_create(clock_id, &clock_sig_event, &timer_id);
        assert(ret == 0);

        /* setting timer interval/initial-expiration values */
        memset(&timer_value, 0, sizeof(struct itimerspec));
        timer_value.it_interval.tv_sec = TIMER_FREQUENCY;
        timer_value.it_interval.tv_nsec = 0;
        timer_value.it_value.tv_sec = TIMER_FREQUENCY;
        timer_value.it_value.tv_nsec = 0;

        /* Create timer */
        ret = timer_settime(timer_id, 0, &timer_value, NULL);
        assert(ret == 0);

        /*
         *  Now when user gives SIGALRM signal then sigalrm_received will be executed when timer
         * gets expired the timer_expired function will be executed.
         * 
         * timer_expired() will executed after every FREQUENCY interval. You can restrict it to
         * execute only once by making timer expiry interval zero.
         * 
         * memset(&timer_value, 0, sizeof(struct itimerspec));
         * timer_value.it_value.tv_sec = TIMER_FREQUENCY;
         * timer_value.it_value.tv_nsec = 0;
         *
         * 
         */
        alarm(5);
        while (1);

        exit(EXIT_SUCCESS);
}
