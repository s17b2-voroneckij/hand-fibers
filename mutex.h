#pragma once

#include "condvar.h"
#include <atomic>

class Mutex {
public:
    void lock();
    void unlock();

private:

    CondVar cv;
    std::atomic<bool> locked = false;
};
