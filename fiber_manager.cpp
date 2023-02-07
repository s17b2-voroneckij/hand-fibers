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
                if (current_fiber->deleting_allowed) {
                    delete current_fiber;
                }
            } else {
                iterator++;
            }
        }
    }
}

void FiberManager::registerFiber(FiberImpl* fiber_ptr) {
    ready_fibers.push_back(fiber_ptr);
}
