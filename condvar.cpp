#include "condvar.h"
#include "fiber_impl.h"
#include "fiber_manager.h"

extern FiberManager fiberManager;
extern thread_local FiberImpl* current_fiber;

void CondVar::wait() {
    auto ret = waiters.push(current_fiber);
    if (!ret) {
        std::cerr << "in CondVar::wait() push failed" << std::endl;
        exit(0);
    }
    current_fiber->is_ready = false;
    current_fiber->suspend();
}

void CondVar::notify_one() {
    FiberImpl* fiber = nullptr;
    if (waiters.pop(fiber)) {
        fiber->is_ready = true;
    }
}

void CondVar::notify_all() {
    FiberImpl* fiber = nullptr;
    while (waiters.pop(fiber)) {
        fiber->is_ready = true;
    }
}

CondVar::CondVar(): waiters(1) {
}
