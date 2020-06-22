#include <iostream>
#include <thread>
#include "buffor.h"
void producer(Buffer &q){
    for(int i = 0; i < 1000; i++){
        std::cout << "Producent " << q.label << " dodaje element do kolejki." << std::endl;
        Message * msg = new Message;
        msg->priority=NORM;
        msg->next= nullptr;
        for(int k = 0; k < 3; k++) {
            msg->text += letters[rand() % 3];}
        q.push(msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}

void consumer(Buffer &q_from, Buffer &q1, Buffer &q2, Buffer &q3, int pr){
    int protect = 0;
    for(int i = 0; i < 1000; i++){
        std::cout << "Konsument " << q_from.label << " odbiera wiadomość." << std::endl;
        Message * msg = q_from.pop();
        std::string str = msg->text;
        if(msg->priority == SPEC) {
            protect = 5;
	}
        else if(protect == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        if(protect > 0) {
            protect--;
        }


        if(!msg->text.empty()){
            char letter = msg->text[0];
            msg->text.erase(0, 1);
            int prob = rand() % 100+1;
            if(prob < pr){
                msg->text += letters[rand() % 3];
            }
            msg->priority=NORM;
            msg->next= nullptr;
            if(letter == 'A'){
                std::cout << "Konsument " << q_from.label << " dodaje wiadomość do kolejki " << q1.label << std::endl;
                q1.push(msg);
            } 
	    else if(letter == 'B'){
                std::cout << "Konsument " << q_from.label << " dodaje wiadomość do kolejki " << q2.label << std::endl;
                q2.push(msg);
            }
            else if(letter == 'C'){
                std::cout << "Konsument " << q_from.label << " dodaje wiadomość do kolejki " << q3.label << std::endl;
                q3.push(msg);
            }
        }else {delete msg;}
    }
}
void priProducer(Buffer &q1, Buffer &q2, Buffer &q3){
    for(int i = 0; i < 1000; i++){
        int which = rand() % 3;
        Message * msg=new Message;
        msg->priority= HIGH;
        msg->next= nullptr;
        msg->text.clear();
        for(int k = 0; k < 3; k++) {
            msg->text += letters[rand() % 3];
        }
        if(which == 0){
            std::cout << "Super producent dodaje element do kolejki " << q1.label << std::endl;
            q1.push(msg);
        } else if(which == 1){
            std::cout << "Super producent dodaje element do kolejki " << q2.label << std::endl;
            q2.push(msg);
        } else{
            std::cout << "Super producent dodaje element do kolejki " << q3.label << std::endl;
            q3.push(msg);
        }
        std::cout << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}
void safeProducer(Buffer &q1, Buffer &q2, Buffer &q3, int wait){
    for(int i = 0; i < 1000; i++){
    Message* msg= new Message;
    msg->text="";
    msg->next= nullptr;
    msg->priority= SPEC;

        if(q1.buffer_occupancy > q2.buffer_occupancy && q1.buffer_occupancy > q3.buffer_occupancy){
            std::cout << "Producent ochronny dodaje pusty komunikat do " << q1.label << std::endl;
            q1.push(msg);
        } else if(q2.buffer_occupancy > q1.buffer_occupancy && q2.buffer_occupancy > q3.buffer_occupancy){
            std::cout << "Producent ochronny dodaje pusty komunikat do " << q2.label << std::endl;
            q2.push(msg);
        } else if(q3.buffer_occupancy > q1.buffer_occupancy && q3.buffer_occupancy > q2.buffer_occupancy){
            std::cout << "Producent ochronny dodaje pusty komunikat do " << q3.label << std::endl;
            q3.push(msg);
        }
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(wait));
    }
}

int main() {
    srand(time(nullptr));
    Buffer queueA('A');
    Buffer queueB('B');
    Buffer queueC('C');
    std::thread prodA(producer, std::ref(queueA));
    usleep(500);
    std::thread prodB(producer, std::ref(queueB));
    usleep(500);
    std::thread prodC(producer, std::ref(queueC));
    usleep(500);
    std::thread pProd(priProducer, std::ref(queueA), std::ref(queueB), std::ref(queueC));
    usleep(500);
    std::thread consA(consumer, std::ref(queueA), std::ref(queueA), std::ref(queueB), std::ref(queueC), PROB);
    usleep(500);
    std::thread consB(consumer, std::ref(queueB), std::ref(queueA), std::ref(queueB), std::ref(queueC), PROB);
    usleep(500);
    std::thread consC(consumer, std::ref(queueC), std::ref(queueA), std::ref(queueB), std::ref(queueC), PROB);
    usleep(500);
    std::thread oProd(safeProducer, std::ref(queueA), std::ref(queueB), std::ref(queueC), 1000);
    prodA.join();
    prodB.join();
    prodC.join();
    pProd.join();
    consA.join();
    consB.join();
    consC.join();
    oProd.join();
    return 0;
}
