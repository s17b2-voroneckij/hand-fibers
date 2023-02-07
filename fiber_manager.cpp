#include "fiber_manager.h"
#include "fiber.hpp"
#include "finisher.hpp"

#include <thread>
#include <chrono>

FiberManager fiberManager;

thread_local FiberImpl* current_fiber;

void FiberManager::work() {
    while (!Finisher::finish) {
        FiberImpl* fiber = nullptr;
        if (ready_fibers.pop(fiber)) {
            current_fiber = fiber;
            if (current_fiber->isReady() && !current_fiber->isFinished()) {
                current_fiber->continue_executing();
            } else {
                std::cerr << "a not ready fiber in FiberManager" << std::endl;
            }
            if (current_fiber->isFinished() && current_fiber->deleting_allowed) {
                std::cerr << "fiber manager deleting fiber" << std::endl;
                delete current_fiber;
            } else if (current_fiber->isReady() && !current_fiber->isFinished()) {
                auto ret = ready_fibers.push(current_fiber);
                if (!ret) {
                    std::cerr << "fiberManager failed to push to queue" << std::endl;
                    exit(0);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void FiberManager::registerFiber(FiberImpl* fiber_ptr) {
    auto ret = ready_fibers.push(fiber_ptr);
    if (!ret) {
        std::cerr << "registerFiber failed while pushing into the queue" << std::endl;
    }
}

FiberManager::FiberManager() : ready_fibers(10) {
}