#pragma once

#include <memory>
#include "fiber_impl.h"
#include "context.h"

using std::shared_ptr;

void startFiberManager();

class FiberManager {
    friend class Fiber;
    friend class FiberImpl;
    friend class CondVar;
    friend void sched_execution();
    void work();
    void registerFiber(const shared_ptr<FiberImpl>& fiber_ptr);

    list<shared_ptr<FiberImpl>> ready_fibers;

    friend void startFiberManager();

    Context manager_context;
};