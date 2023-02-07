#pragma once

#include <memory>
#include <boost/lockfree/queue.hpp>
#include "fiber_impl.h"

void startFiberManager();

class FiberManager {
public:
    FiberManager();

private:
    friend class Fiber;
    friend class FiberImpl;
    friend class CondVar;
    friend void sched_execution();
    void work();
    void registerFiber(FiberImpl* fiber_ptr);

    boost::lockfree::queue<FiberImpl*> ready_fibers;

    friend void startFiberManager();
};