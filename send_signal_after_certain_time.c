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

/* 24h = 24 * 60 * 60*/
#define TIMER_FREQUENCY 86400

void timer_expired(int sig)
{
        // use this handler
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
        act.sa_handler =  timer_expired;
        ret = sigaction(SIGUSR1, &act, NULL);
        assert(ret == 0);

        clock_id = CLOCK_MONOTONIC;
        memset(&clock_sig_event, 0, sizeof( struct sigevent));
        clock_sig_event.sigev_notify = SIGEV_SIGNAL;
        clock_sig_event.sigev_signo = SIGUSR1;
        clock_sig_event.sigev_notify_attributes = NULL;
        /* Creating process interval timer */
        ret = timer_create(clock_id, &clock_sig_event, &timer_id);
        assert(ret == 0);

        /* setitng timer interval values */
        memset(&timer_value, 0, sizeof(struct itimerspec));
        timer_value.it_interval.tv_sec = TIMER_FREQUENCY;
        timer_value.it_interval.tv_nsec = 0;

        /* setting timer initial expiration values*/
        timer_value.it_value.tv_sec = TIMER_FREQUENCY;
        timer_value.it_value.tv_nsec = 0;

        /* Create timer */
        ret = timer_settime(timer_id, 0, &timer_value, NULL);
        assert(ret == 0);

        /* Now we have a timer with following features:
         * It will expire after 24 hours and excute fucntion timer_expired
         * upon expiration */

        /*
         * Do something here
         *
         * To test, sleep()  for 5 sec and set timer expiry as 1 sec i.e.
         * TIMER_FREQUENCY as 1.
         * In this case first timer will expire after 1 sec and sleep will
         * come out, also timer_expired() will be executed. Later time will expire after every 1 sec.
         *
         * We can also use SIGEV_THREAD instead of SIGEV_SIGNAL in clock_sig_event.sigev_notify
         * while creating timer_id using timer_create().In that case we need to check for other parameter
         * as well.
         */

        exit(EXIT_SUCCESS);
}
