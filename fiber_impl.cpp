#include "fiber_impl.h"
#include "fiber_manager.h"

using std::cerr;

extern FiberManager fiberManager;

extern thread_local FiberImpl* current_fiber;

FiberImpl::FiberImpl(const std::function<void()> &func) {
    cerr << "new fiber being created" << std::endl;
    this->func = func;
}

void FiberImpl::join() {
    while (!finished) {
        finish_cv.wait();
    }
}

bool FiberImpl::isFinished() {
    return finished;
}

void FiberImpl::start() {
    fiberManager.registerFiber(this);
    is_ready = true;
}

void FiberImpl::continue_executing() {
    if (!launched) {
        launched = true;
        this_context = callcc([&](auto sink) {
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

FiberImpl::~FiberImpl() {
    cerr << "fiber being deleted" << std::endl;
}
