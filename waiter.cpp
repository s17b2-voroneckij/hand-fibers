#include "waiter.hpp"
#include "fiber.hpp"

#include <vector>
#include <poll.h>
#include <cstring>

Waiter waiter;

int Waiter::wait(int fd, short events) {
    FdRequest fdRequest{.cv = CondVar(), .fd = fd, .events = events};
    //waiter.map_mutex.lock();
    waiter.map[fd] = &fdRequest;
    //waiter.map_mutex.unlock();
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
        //waiter.map_mutex.lock();
        while (!waiter.stopped && waiter.map.empty()) {
            //waiter.map_mutex.unlock();
            waiter.cv.wait();
            //waiter.map_mutex.lock();
        }
        if (waiter.stopped) {
            //waiter.map_mutex.unlock();
            std::cerr << "Waiter fiber leaving with break\n";
            break;
        }
        std::vector<pollfd> request;
        request.reserve(waiter.map.size());
        for (auto& elem : waiter.map) {
            request.push_back(pollfd{.fd = elem.first, .events = elem.second->events, .revents = 0});
        }
        //waiter.map_mutex.unlock();
        int ret = poll(&request[0], request.size(), 0);
        if (ret < 0) {
            printf("poll returned with error %s", strerror(errno));
            continue;
        }
        //waiter.map_mutex.lock();
        for (auto& elem : request) {
            if (elem.revents > 0) {
                waiter.map[elem.fd]->cv.notify_one();
                waiter.map.erase(elem.fd);
            }
        }
        //waiter.map_mutex.unlock();
        sched_execution();
    }
    std::cerr << "waiter main loop leaving\n";
    //waiter.map_mutex.lock();
    for (auto& elem : waiter.map) {
        std::cerr << "waiter main loop notifying someone before leaving\n";
        //elem.second->cv.notify_all();
    }
    //waiter.map_mutex.unlock();
    std::cerr << "waiter finally leaving\n";
}
