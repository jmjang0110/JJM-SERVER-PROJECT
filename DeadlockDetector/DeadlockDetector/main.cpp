#include "DeadlockDetector.h"
#include "LockTest.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>

Lock A("A");
Lock B("B");
Lock C("C");

constexpr int CNT = 20000;
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;

void Thread_A() {
    for (int i = 0; i < CNT; ++i) {
        A.lock();
        a_cnt += 1;
        {
            B.lock();
            b_cnt += 1;
            {
                C.lock();
                c_cnt += 1;
                C.unlock();
            }
            B.unlock();
        }
        A.unlock();
    }
}

void Thread_B() {
    for (int i = 0; i < CNT; ++i) {
        B.lock();
        b_cnt += 1;
        {
            C.lock();
            c_cnt += 1;
            {
                A.lock();
                a_cnt += 1;
                A.unlock();
            }
            C.unlock();
        }
        B.unlock();
    }
}

void Thread_C() {
    for (int i = 0; i < CNT; ++i) {
        C.lock();
        c_cnt += 1;
        {
            A.lock();
            a_cnt += 1;
            {
                B.lock();
                b_cnt += 1;
                B.unlock();
            }
            A.unlock();
        }
        C.unlock();
    }
}

int main(void) {
    std::thread tA(Thread_A);
    std::thread tB(Thread_B);
    std::thread tC(Thread_C);

    tA.join();
    tB.join();
    tC.join();

    return 0;
}
