#include "fiber_manager.h"
#include "fiber.hpp"
#include "finisher.hpp"

FiberManager fiberManager;

thread_local FiberImpl* current_fiber;

void FiberManager::work() {
    while (!ready_fibers.empty()) {
        auto iterator = this->ready_fibers.begin();
        while (iterator != ready_fibers.end()) {
            current_fiber = *iterator;
            if (current_fiber->isReady()) {
                current_fiber->continue_executing();
            }
            if (current_fiber->isFinished()) {
                iterator = ready_fibers.erase(iterator);
            } else {
                iterator++;
            }
        }
    }
}

void FiberManager::registerFiber(FiberImpl* fiber_ptr) {
    ready_fibers.push_back(fiber_ptr);
    auto ret = all_fibers.push(fiber_ptr);
    if (!ret) {
        std::cerr << "adding to queue failed, leaving" << std::endl;
        exit(0);
    }
}

FiberManager::FiberManager(): all_fibers(10) {
}

void deletionFunction() {
    while (!Finisher::finish) {
        queue<FiberImpl*> temp(10);
        FiberImpl *this_fiber = nullptr;
        int iter = 0;
        while (fiberManager.all_fibers.pop(this_fiber)) {
            if (this_fiber->deleting_allowed && this_fiber->finished) {
                delete this_fiber;
            } else {
                temp.unsynchronized_push(this_fiber);
            }
            iter++;
        }
        while (temp.unsynchronized_pop(this_fiber)) {
            fiberManager.all_fibers.push(this_fiber);
        }
        if (iter == 0) {
            fiberManager.deletion_cv.wait();
        }
        sched_execution();
    }
}
namespace {
    Fiber fiber(deletionFunction);
}