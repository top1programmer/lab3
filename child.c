/* child.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int a;
    int b;
} pair_t;

volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t interrupt_count = 0;
volatile pair_t cur = {0, 0};

volatile sig_atomic_t count_00 = 0;
volatile sig_atomic_t count_01 = 0;
volatile sig_atomic_t count_10 = 0;
volatile sig_atomic_t count_11 = 0;

char *child_name;
struct timeval last_print = {0};

void print_stats() {   
    struct timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec > last_print.tv_sec || 
       (now.tv_sec == last_print.tv_sec && now.tv_usec > last_print.tv_usec)) {
        printf("PID: %-5d Name: %-8s {0,0}=%-3d {0,1}=%-3d {1,0}=%-3d {1,1}=%-3d\n",
               getpid(), child_name, count_00, count_01, count_10, count_11);
        last_print = now;
    }
}

void timer_handler(int signum) {
    (void)signum;
    pair_t counter = {cur.a, cur.b};

    if (counter.a == 0 && counter.b == 0) count_00++;
    else if (counter.a == 0 && counter.b == 1) count_01++;
    else if (counter.a == 1 && counter.b == 0) count_10++;
    else if (counter.a == 1 && counter.b == 1) count_11++;

    if (++interrupt_count >= 10) {
        print_stats();
        interrupt_count = 0;
        count_00 = count_01 = count_10 = count_11 = 0;
    }
}

void sig_handler(int signum) {
    switch(signum) {
        case SIGTERM:
            keep_running = 0;
            break;
        case SIGUSR1:
            print_stats();
            break;
    }
}

void setup_timer(void) {
    struct sigaction sa = {0};
    sa.sa_handler = timer_handler;
    sigaction(SIGALRM, &sa, NULL);

    struct sigaction sa_term = {0};
    sa_term.sa_handler = sig_handler;
    sigaction(SIGTERM, &sa_term, NULL);
    sigaction(SIGUSR1, &sa_term, NULL);

    struct itimerval timer = {
        .it_value = {.tv_sec = 1, .tv_usec = 0},
        .it_interval = {.tv_sec = 1, .tv_usec = 0}
    };
    setitimer(ITIMER_REAL, &timer, NULL);
}

int main(int argc, char *argv[]) {
    setup_timer();
    child_name = argv[0];
    
    while(keep_running) {
        cur.a = 0; 
        cur.b = 0;
        usleep(50);
        
        cur.a = 1; 
        usleep(50);
        
        cur.b = 1;
        usleep(50);
        
        usleep(100);
    }

    struct itimerval timer = {0};
    setitimer(ITIMER_REAL, &timer, NULL);
    printf("%s (PID: %d) terminated\n", child_name, getpid());
    return 0;
}