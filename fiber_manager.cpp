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
            fiber->in_queue = false;
            //std::cerr << "worker popped Fiber " << fiber << std::endl;
            current_fiber = fiber;
            if (current_fiber->isReady() && !current_fiber->isFinished() && !current_fiber->executing.exchange(true)) {
                //std::cerr << "worker starting Fiber at " << current_fiber << std::endl;
                current_fiber->continue_executing();
                //std::cerr << "worker done executing Fiber at " << current_fiber << std::endl;
                current_fiber->executing = false;
            } else {
                //std::cerr << "a not ready fiber in FiberManager" << std::endl;
            }
            if (current_fiber->isFinished() && current_fiber->deleting_allowed) {
                //std::cerr << "fiber manager deleting fiber" << std::endl;
                delete current_fiber;
            } else if (current_fiber->isReady() && !current_fiber->isFinished()) {
                registerFiber(current_fiber);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void FiberManager::registerFiber(FiberImpl* fiber_ptr) {
    if (!fiber_ptr->in_queue.exchange(true)) {
        //std::cerr << "registerFiber pushing fiber " << fiber_ptr << std::endl;
        auto ret = ready_fibers.push(fiber_ptr);
        if (!ret) {
            //std::cerr << "registerFiber failed while pushing into the queue" << std::endl;
        }
    }
}

FiberManager::FiberManager() : ready_fibers(10) {
}

void start_fiber_manager_thread() {
    std::thread thread([] {
        fiberManager.work();
    });
    thread.detach();
}