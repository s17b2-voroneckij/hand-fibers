#pragma once

#include <algorithm>
#include <atomic>
#include <iostream>
#include <memory>
#include <list>
#include <boost/context/continuation.hpp>
#include "condvar.h"

using namespace boost::context;

class FiberManager;

void sched_execution();

class FiberImpl {
public:
    ~FiberImpl();

private:
    FiberImpl(const std::function<void()>& func);
    void join();
    bool isFinished();
    void start();
    bool isReady() const;
    friend class Fiber;
    friend class FiberManager;
    friend class CondVar;
    friend void sched_execution();
    void continue_executing();
    void suspend();

    std::function<void()> func;
    continuation this_context;
    continuation previous_context;
    CondVar finish_cv;
    std::atomic<bool> launched = false;
    std::atomic<bool> finished = false;
    std::atomic<bool> is_ready = false;
    std::atomic<bool> deleting_allowed = false;
    std::atomic<bool> in_queue = false;
    std::atomic<bool> executing = false;
};
