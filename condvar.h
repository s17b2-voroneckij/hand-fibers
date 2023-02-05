#pragma once

#include <vector>

#include "fiber_impl.h"

class CondVar {
public:
    void wait();
    void notify_one();
    void notify_all();

private:
    std::vector<shared_ptr<FiberImpl>> waiters;
};