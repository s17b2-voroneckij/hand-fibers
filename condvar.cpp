#include "condvar.h"
#include "fiber_impl.h"
#include "fiber_manager.h"

extern FiberManager fiberManager;
extern thread_local FiberImpl* current_fiber;

void CondVar::wait() {
    waiters.push_back(current_fiber);
    current_fiber->is_ready = false;
    current_fiber->suspend();
}

void CondVar::notify_one() {
    if (!waiters.empty()) {
        auto fiber_ptr = *waiters.rbegin();
        fiber_ptr->is_ready = true;
        waiters.pop_back();
    }
}

void CondVar::notify_all() {
    for (auto fiber_ptr : waiters) {
        fiber_ptr->is_ready = true;
    }
}
