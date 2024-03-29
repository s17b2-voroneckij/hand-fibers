#include <iostream>
#include <vector>

#include "../fiber/fiber_impl.h"
#include "../sync/condvar.h"
#include "../sync/mutex.h"
#include "../fiber/fiber.hpp"
#include "../waiter/waiter.hpp"

using std::cerr;
using std::endl;
using std::vector;

void test_func() {
    static int global_id = 0;
    int this_id = global_id++;
    cerr << "test_func entering " << this_id << endl;
    sched_execution();
    cerr << "test_func leaving " << this_id << endl;
}

void ping_pong() {
    static bool global_cnt = false;
    static CondVar cv;
    auto allowed = global_cnt;
    global_cnt = !global_cnt;
    for (int i = 0; i < 5; i++) {
        if (allowed) {
            cerr << "notify" << endl;
            cv.notify_one();
            allowed = false;
        } else {
            cerr << "waiting" << endl;
            cv.wait();
            cerr << "waited" << endl;
            allowed = true;
        }
    }
}

Mutex m;
int inside = 0;

void test_mutex() {
    static int counter = 0;
    int id = counter++;
    for (int i = 0; i < 5; i++) {
        cerr << id << " going to lock" << endl;
        m.lock();
        inside++;
        cerr << id << " inside: " << inside << endl;
        sched_execution();
        inside--;
        m.unlock();
        sched_execution();
    }
}

void print_number(int a) {
    cerr << a << endl;
}

int test_main() {
    Fiber fiber(print_number, 8);
    fiber.join();

    int number_of_threads = 10;
    vector<Fiber> threads;
    threads.reserve(number_of_threads);
    for (int i = 0; i < number_of_threads; i++) {
        threads.emplace_back(test_mutex);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
    /*
    FiberImpl ping(ping_pong), pong(ping_pong);
    ping.start();
    pong.start();
    ping.join();
    pong.join();
    return 0;

    FiberImpl fiber(test_func), fiber2(test_func), fiber3(test_func);
    fiber.start();
    fiber2.start();
    fiber3.start();
    fiber.join();
    fiber2.join();
    fiber3.join();
     */
}