#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <list>
#include "../sync/condvar.h"
#include "../context/context.h"

using std::cerr;
using std::shared_ptr;
using std::list;

class FiberManager;

class CondVar;

void sched_execution();

class FiberImpl: public std::enable_shared_from_this<FiberImpl> {
private:
    explicit FiberImpl(const std::function<void()>& func);
    void join();
    bool isFinished() const;
    void start();
    bool isReady() const;
    friend class Fiber;
    friend class FiberManager;
    friend class CondVar;
    friend void sched_execution();
    friend void fiber_starter();
    void continue_executing();
    void suspend();
    static void fiber_starter(FiberImpl*);

    std::function<void()> func;
    Context this_context;
    Context* previous_context;
    CondVar finish_cv;
    bool launched = false;
    bool finished = false;
    bool is_ready = false;
};
