#include "fiber_impl.h"
#include "fiber_manager.h"

extern FiberManager fiberManager;

extern std::shared_ptr<FiberImpl> current_fiber;

FiberImpl::FiberImpl(const std::function<void()> &func) {
    this->func = func;
}

void FiberImpl::join() {
    while (!finished) {
        finish_cv.wait();
    }
}

bool FiberImpl::isFinished() const {
    return finished;
}

void FiberImpl::start() {
    fiberManager.registerFiber(this->shared_from_this());
    is_ready = true;
}

void FiberImpl::continue_executing() {
    if (!launched) {
        launched = true;
        this_context = callcc([&](auto sink) {
            cerr << "starting func in new fiber\n";
            previous_context = std::move(sink);
            func();
            finished = true;
            finish_cv.notify_all();
            return std::move(previous_context);
        });
    } else {
        this_context = this_context.resume();
    }
}

void FiberImpl::suspend() {
    previous_context = previous_context.resume();
}

void sched_execution() {
    current_fiber->suspend();
}

void startFiberManager() {
    fiberManager.work();
}

bool FiberImpl::isReady() const {
    return is_ready;
}
