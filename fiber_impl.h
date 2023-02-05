#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <list>
#include <boost/context/continuation.hpp>

using namespace boost::context;
using std::cerr;
using std::shared_ptr;
using std::list;

class FiberManager;

class CondVar;

void sched_execution();

class FiberImpl: public std::enable_shared_from_this<FiberImpl> {
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
    bool launched = false;
    bool finished = false;
    bool is_ready = false;
};
