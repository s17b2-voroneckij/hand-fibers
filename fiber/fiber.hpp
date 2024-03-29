#pragma once

#include "fiber_impl.h"
#include "fiber_manager.h"

extern FiberManager fiberManager;

class Fiber {
public:
    template<typename Callable, typename... Args>
    explicit Fiber(const Callable& function, const Args&... args) {
        fiber_ptr = shared_ptr<FiberImpl>(new FiberImpl([&] () {
            function(args...);
        }));
        fiber_ptr->start();
    }

    void join() {
        fiber_ptr->join();
    }

    void detach() {
        fiberManager.work();
    }

    bool isFinished() {
        return fiber_ptr->isFinished();
    }

    bool isReady() {
        return fiber_ptr->isReady();
    }

private:
    shared_ptr<FiberImpl> fiber_ptr;
};