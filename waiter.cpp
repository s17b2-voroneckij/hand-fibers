#include "waiter.hpp"
#include "fiber.hpp"

#include <vector>
#include <poll.h>
#include <cstring>

Waiter waiter;

int Waiter::wait(int fd, short events) {
    FdRequest fdRequest{.cv = CondVar(), .fd = fd, .events = events};
    waiter.map[fd] = &fdRequest;
    waiter.cv.notify_one();
    fdRequest.cv.wait();
    return fdRequest.events;
}

Waiter::Waiter() {
    std::cout << "waiter initialising" << std::endl;
}

Fiber fiber(Waiter::loop);

void Waiter::stop() {
    waiter.stopped = true;
}

void Waiter::loop() {
    while (!waiter.stopped) {
        while (!waiter.stopped && waiter.map.empty()) {
            waiter.cv.wait();
        }
        if (waiter.stopped) {
            break;
        }
        std::vector<pollfd> request;
        request.reserve(waiter.map.size());
        for (auto& elem : waiter.map) {
            request.push_back(pollfd{.fd = elem.first, .events = elem.second->events, .revents = 0});
        }
        int ret = poll(&request[0], request.size(), 100);
        if (ret < 0) {
            printf("poll returned with error %s", strerror(errno));
            continue;
        }
        for (auto& elem : request) {
            if (elem.revents > 0) {
                waiter.map[elem.fd]->cv.notify_one();
                waiter.map.erase(elem.fd);
            }
        }
        sched_execution();
    }
    cerr << "waiter stopped, going to notify all" << std::endl;
    for (auto& elem : waiter.map) {
        elem.second->cv.notify_all();
    }
}
