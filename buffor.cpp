
#include <iostream>
#include "buffor.h"

Buffer::Buffer(char l)  {
    head = pri_tail = spec_tail = tail = nullptr;
    buffer_occupancy = 0;
    label=l;
};
Buffer::~Buffer() {
    std::unique_lock<std::mutex> lock(my_mutex);
    while(head){
        Message * del = this->pop();
        delete del;
    }
    lock.unlock();
}
Message * Buffer::pop() {
    enter();
    std::unique_lock<std::mutex> lock(my_mutex);
    if(head == nullptr){
        std::time_t result = std::time(nullptr);
        std::cout << result << ": Kolejka "<< label << " jest pusta" << std::endl << std::endl;
        lock.unlock();
        wait(cons);
        lock.lock();
    }
    Message * msg;
    msg = head;
    if (tail == head) {tail = nullptr;}
    if (pri_tail == head) {pri_tail = nullptr;}
    if (spec_tail == head) {spec_tail = nullptr;}
    head = head->next;
    buffer_occupancy--;
    std::time_t result = std::time(nullptr);
    std::cout << result << " odczytano: " << msg->text << " o priorytecie: " << msg->priority
    <<" z kolejki " << label <<std::endl << std::endl;
    lock.unlock();
    signal(prod);
    leave();
    return msg;
}
void Buffer :: push_norm (Message * msg){
    if(head) {
        tail->next = msg;
        tail = msg;
        msg->next = nullptr;
    }
    else {
        head=msg;
        tail=msg;
    }
}
void Buffer :: push_pri(Message * msg){
    if (head) {
        if(pri_tail) {
            msg->next = pri_tail->next;
            pri_tail->next = msg;
            pri_tail = msg;
        }
        else if(spec_tail){
          msg->next = spec_tail->next;
          spec_tail->next = msg;
          pri_tail=msg;
        }
        else{
            msg->next = head;
            head = msg;
            pri_tail=msg;
        }
    }
    else{
        head=msg;
        pri_tail=msg;
	tail = msg; 
    }
}

void Buffer :: push_spec(Message * msg){
    if (head) {
        if(spec_tail){
            msg->next = spec_tail->next;
            spec_tail->next = msg;
            spec_tail=msg;
        }
        else{
            msg->next = head;
            head = msg;
            spec_tail=msg;
        }
    }
    else{
        head=msg;
        spec_tail=msg;
	tail = msg;
    }
}

void Buffer :: push(Message * msg){
    std::unique_lock<std::mutex> lock(my_mutex);
    enter();
    if(buffer_occupancy == SIZE){
        std::time_t result = std::time(nullptr);
        std::cout << result << ": Kolejka "<< label <<" jest pelna" << std::endl << std::endl;
        lock.unlock();
        wait(prod);
        lock.lock();
    }
    std::time_t result = std::time(nullptr);
    std::cout << result << " dodano: " << msg->text << " o priorytecie "<< msg->priority << " do kolejki "<< label
    << std::endl<< std::endl;
    if (msg->priority== NORM){
        push_norm(msg);
    }
    else if (msg->priority== HIGH){
        push_pri(msg);
    }
    else if (msg->priority==SPEC){
        push_spec(msg);
    }
    buffer_occupancy++;
    lock.unlock();
    signal(cons);
    leave();
};
