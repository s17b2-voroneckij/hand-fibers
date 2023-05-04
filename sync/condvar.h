#pragma once

#include <vector>
#include <memory>

using std::shared_ptr;

class FiberImpl;

class CondVar {
public:
    void wait();
    void notify_one();
    void notify_all();

private:
    std::vector<shared_ptr<FiberImpl>> waiters;
};