#include "fiber_manager.h"

FiberManager fiberManager;

thread_local std::shared_ptr<FiberImpl> current_fiber;

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

void FiberManager::registerFiber(const shared_ptr<FiberImpl>& fiber_ptr) {
    ready_fibers.push_back(fiber_ptr);
}
