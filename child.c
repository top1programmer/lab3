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

// Счетчики состояний
volatile sig_atomic_t count_00 = 0;
volatile sig_atomic_t count_01 = 0;
volatile sig_atomic_t count_10 = 0;
volatile sig_atomic_t count_11 = 0;

void print_stats() {    
    printf("{0,0}=%d {0,1}=%d {1,0}=%d {1,1}=%d\n",
           count_00, count_01, count_10, count_11);

}

void timer_handler(int signum) {
    (void)signum;
    // Фиксируем текущее состояние
    pair_t counter = {cur.a, cur.b};

    // Обновляем счетчики
    if (counter.a == 0 && counter.b == 0) count_00++;
    else if (counter.a == 0 && counter.b == 1) count_01++;
    else if (counter.a == 1 && counter.b == 0) count_10++;
    else if (counter.a == 1 && counter.b == 1) count_11++;

    if (++interrupt_count >= 100) {
        print_stats();
        //keep_running = 0;
        interrupt_count = 0;
        count_00 = count_01 = count_10 = count_11 = 0;
    }
}

void sigint_handler(int signum) {
    (void)signum;
    keep_running = 0;
}

void setup_timer(void) {
    struct sigaction sa = {0};
    sa.sa_handler = timer_handler;
    sigaction(SIGALRM, &sa, NULL);

    signal(SIGINT, sigint_handler);

    struct itimerval timer = {
        .it_value = {.tv_sec = 1, .tv_usec = 0},
        .it_interval = {.tv_sec = 1, .tv_usec = 0}
    };
    setitimer(ITIMER_REAL, &timer, NULL);
}

int main(void) {
    setup_timer();
    
    printf("Программа запущена. Собираем статистику 100 прерываний...\n");
    printf("Нажмите Ctrl+C для досрочного завершения\n");

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

    // Отключаем таймер
    struct itimerval timer = {0};
    setitimer(ITIMER_REAL, &timer, NULL);

    
    printf("Программа завершена\n");
    return 0;
}