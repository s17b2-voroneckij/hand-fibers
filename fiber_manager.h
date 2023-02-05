#pragma once

#include <memory>
#include "fiber_impl.h"

using std::shared_ptr;

class FiberManager {
    friend class Fiber;
    friend class FiberImpl;
    friend class CondVar;
    friend void sched_execution();
    void work();
    void registerFiber(const shared_ptr<FiberImpl>& fiber_ptr);

    list<shared_ptr<FiberImpl>> ready_fibers;
};