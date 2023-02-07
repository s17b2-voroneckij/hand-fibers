#pragma once

#include "fiber_impl.h"
#include "fiber_manager.h"

extern FiberManager fiberManager;

class Fiber {
public:
    template<typename Callable, typename... Args>
    Fiber(const Callable& function, const Args&... args) {
        fiber_ptr = new FiberImpl([args..., function] () {
            function(args...);
        });
        fiber_ptr->start();
    }

    ~Fiber() {
        fiber_ptr->deleting_allowed = true;
        if (fiber_ptr->isFinished()) {
            std::cerr << "~Fiber() deleting fiber" << std::endl;
            delete fiber_ptr;
        }
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
    FiberImpl* fiber_ptr;
};