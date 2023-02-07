#pragma once

#include <memory>
#include <boost/lockfree/queue.hpp>
#include "fiber_impl.h"

void startFiberManager();

void start_fiber_manager_thread();

class FiberManager {
public:
    FiberManager();

private:
    friend class Fiber;
    friend class FiberImpl;
    friend class CondVar;
    friend void sched_execution();
    friend void start_fiber_manager_thread();
    void work();
    void registerFiber(FiberImpl* fiber_ptr);

    boost::lockfree::queue<FiberImpl*> ready_fibers;

    friend void startFiberManager();
};