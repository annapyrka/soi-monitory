
#ifndef _BUFFOR_H
#define _BUFFOR_H

#include <mutex>
#include "monitor.h"
#define SIZE 20
#define NORM 0
#define HIGH 1
#define SPEC 2
#define PROB 70
const char letters[] = "ABC";

struct Message {
    std :: string text;
    int priority;
    Message * next;
};
class Buffer : private Monitor {
private:
    Message *head, *pri_tail, *spec_tail, *tail;
    Condition prod, cons;
    std::mutex my_mutex;

public:
    int buffer_occupancy;
    char label;
    Buffer()  {
        head = pri_tail = spec_tail = tail = nullptr;
        buffer_occupancy = 0;
        label='Z';
    };
    Buffer(char);
    ~Buffer();
    Message * pop();
    void push(Message *m);
    void push_norm (Message * m);
    void push_pri (Message *m);
    void push_spec (Message *m);
};

#endif //_BUFFOR_H
