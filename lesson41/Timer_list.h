#ifndef LST_TIMER
#define LST_TIMER

#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <signal.h>
#define BUFFER_SIZE 64

class util_timer;

struct client_data {
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer *timer;
};

class util_timer {
public: 
    util_timer() : prev(nullptr), next(nullptr) {}
    time_t expire;
    void (*cb_func)(client_data *);
    client_data *user_data;
    util_timer *prev;
    util_timer *next;
};

class sort_timer_list {
public:
    sort_timer_list() : head(nullptr), tail(nullptr) {}

    ~sort_timer_list() {
        util_timer *curr = head;
        while (curr) {
            util_timer *next = curr->next;
            delete curr;
            curr = next;
        }
    }

    void add_timer(util_timer *timer) {
        if (!timer) {
            return;
        }
        if (!head) {
            head = tail = timer;
            return;
        }
        if (timer->expire < head->expire) {
            timer->next = head;
            head->prev = timer;
            head = timer;
            return;
        }
        add_timer(timer, head);
    }

    void adjust_timer(util_timer *timer) {
        if (!timer) {
            return;
        }
        if (timer == tail || timer->expire < timer->next->expire) {
            return;
        }
        if (timer == head) {
            head = head->next;
            head->prev = nullptr;
            add_timer(timer, head);

        } else {
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            add_timer(timer, timer->next);
        }
    }

    void del_timer(util_timer *timer) {
        if (!timer) {
            return;
        }
        if (timer == head && timer == tail) {
            head = tail = nullptr;
            delete timer;
            return;
        }
        if (timer == head) {
            head = head->next;
            delete timer;
            head->prev = nullptr;
        }
        if (timer == tail) {
            tail = tail->prev;
            tail->next = nullptr;
            delete timer;
        }

        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        delete timer;
    }

    void tick() {
        if (!head) {
            return;
        }
        time_t cur = time(NULL);
        util_timer *curr = head;
        while (curr) {
            if (curr->expire > cur) {
                break;
            }
            head = head->next;
            if (head) {
                head->prev = nullptr; 
            }
            curr->cb_func(curr->user_data);
            delete curr;
            curr = head;
        }
    }


private:
    void add_timer(util_timer *timer, util_timer *pass_head) {
        util_timer *curr = pass_head->next;
        while (curr) {
            if (curr->expire > timer->expire) {
                util_timer *prev = curr->prev;
                prev->next = timer;
                timer->prev = prev;
                curr->prev = timer;
                timer->next = curr;
                return;
            }
        }

        tail->next = timer;
        timer->prev = tail;
        timer->next = nullptr;
        tail = timer;
        return;
    }
    util_timer *head, *tail;
};

#endif