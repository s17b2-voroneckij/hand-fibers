#pragma once

#include <vector>
#include <boost/lockfree/queue.hpp>

using boost::lockfree::queue;

class FiberImpl;

class CondVar {
public:
    CondVar();
    void wait();
    void notify_one();
    void notify_all();

private:
    queue<FiberImpl*> waiters;
};