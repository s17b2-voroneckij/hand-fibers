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

void FiberImpl::fiber_starter(FiberImpl* this_) {
    this_->func();
    this_->finished = true;
    switch_context(&this_->this_context, this_->previous_context);
    this_->finish_cv.notify_all();
}

void FiberImpl::continue_executing() {
    if (!launched) {
        launched = true;
        this_context = Context::create_context();
        this_context.setRip((unsigned long)(&FiberImpl::fiber_starter));
    }
    // the third argument is passed for the case when fiber_starter is called; it is passed as this_
    switch_context(previous_context, &this_context, reinterpret_cast<unsigned long>(this));
}

void FiberImpl::suspend() {
    switch_context(&this_context, previous_context);
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
