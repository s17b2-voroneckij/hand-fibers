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
    std::cerr << "waiter initialising" << std::endl;
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
            std::cerr << "Waiter fiber leaving with break\n";
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
    std::cerr << "waiter main loop leaving\n";
    for (auto& elem : waiter.map) {
        std::cerr << "waiter main loop notifying someone before leaving\n";
        //elem.second->cv.notify_all();
    }
    std::cerr << "waiter finally leaving\n";
}
