#include <iostream>
#include <vector>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "condvar.h"
#include "mutex.h"
#include "fiber.hpp"
//#include "waiter.hpp"
#include "fiber_manager.h"
#include "context.h"


int main() {
    Fiber fiber1([] () {
        printf("hello from fiber1\n");
        sched_execution();
        printf("fiber1 executing again\n");
        sched_execution();
        printf("fiber1 executing yet again\n");
    });
    Fiber fiber2([] () {
        printf("hello from fiber2\n");
        sched_execution();
        printf("fiber2 executing again\n");
        sched_execution();
        printf("fiber2 executing yet again\n");
    });
    startFiberManager();
    fiber1.join();
    fiber2.join();
    printf("fibers done\n");
}