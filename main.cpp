#include <iostream>
#include <vector>
#include <cstring>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <csignal>

#include "fiber_impl.h"
#include "condvar.h"
#include "mutex.h"
#include "fiber.hpp"
#include "waiter.hpp"
#include "fiber_manager.h"
#include "finisher.hpp"

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

unsigned long fibonacci(long n) {
    if (n <= 2) {
        return 1;
    }
    //if ((long) rand() * rand() % 10000000 == 0) {
    //    sched_execution();
    //}
    return fibonacci(n - 1) + fibonacci(n - 2);
}

template<typename Callable, typename... Args>
auto run_with_timing(Callable function, Args ...args) {
    auto start_time = clock();
    auto result = function(args...);
    std::cout << "completed in " << ((double) clock() - start_time) / CLOCKS_PER_SEC << std::endl;
    return result;
}

void worker(int fd) {
    printf("work called with fd: %d\n", fd);
    char buf[1024];
    while (!Finisher::finish) {
        Waiter::wait(fd, POLLIN);
        if (Finisher::finish) {
            return;
        }
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n == 0) {
            printf("client finished, leaving\n");
            return;
        }
        if (n < 0) {
            printf("in worker error: %s\n", strerror(errno));
            return;
        }
        long request = strtol(buf, nullptr, 10);
        auto response = run_with_timing(fibonacci, request);
        auto response_string = "response: " + std::to_string(response) + "\n";
        strcpy(buf, response_string.c_str());
        n = strlen(buf);
        int wrote = 0;
        while (!Finisher::finish && wrote < n) {
            Waiter::wait(fd, POLLOUT);
            if (Finisher::finish) {
                return;
            }
            ssize_t m = write(fd, buf + wrote, n - wrote);
            if (m < 0) {
                printf("in worker error: %s\n", strerror(errno));
                return;
            }
            wrote += m;
        }
    }
}

void signal_handler(int) {
    char str[] = "stop request received\n";
    write(2, str, sizeof(str) - 1);
    Finisher::finish = true;
    Waiter::stop();
}

int main() {
    std::cerr << "pid: " << getpid() << std::endl;
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGABRT, signal_handler);
    std::cerr << "main enetered" << endl;
    Fiber global_fiber([]() {
        int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_fd < 0) {
            printf("socket error: %s\n", strerror(errno));
            exit(0);
        }
        int ret = fcntl(socket_fd, F_SETFL, O_NONBLOCK);
        if (ret == -1) {
            printf("fcntl error: %s\n", strerror(errno));
            exit(0);
        }
        sockaddr_in sin{};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(8001);
        sin.sin_addr = in_addr{0};
        if (bind(socket_fd, reinterpret_cast<const sockaddr *>(&sin), sizeof(sin)) < 0) {
            printf("bind error: %s\n", strerror(errno));
            exit(0);
        }
        if (listen(socket_fd, 10) < 0) {
            printf("listen error: %s\n", strerror(errno));
            exit(0);
        }
        while (!Finisher::finish) {
            std::cerr << "accepting\n";
            Waiter::wait(socket_fd, POLLIN);
            std::cerr << "main fiber waited on Waiter\n";
            if (Finisher::finish) {
                std::cerr << "main thread leaving after waiting\n";
                break;
            }
            int client_fd = accept4(socket_fd, NULL, NULL, SOCK_NONBLOCK);
            Fiber(worker, client_fd);
        }
        std::cerr << "main thread leaving\n";
    });
    start_fiber_manager_thread();
    startFiberManager();
/*
    Fiber fiber(print_number, 8);
    {
        Fiber fiber2(print_number, 10);
    }
    startFiberManager();

    /*
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
    fiber3.join();*/
}
