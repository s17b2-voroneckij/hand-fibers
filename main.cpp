#include <iostream>
#include <vector>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "sync/condvar.h"
#include "sync/mutex.h"
#include "fiber/fiber.hpp"
#include "waiter/waiter.hpp"
#include "fiber/fiber_manager.h"
#include "context/context.h"

using std::cerr;
using std::endl;
using std::vector;

void worker(int fd) {
    printf("work called with fd: %d\n", fd);
    char buf[1024];
    while (true) {
        Waiter::wait(fd, POLLIN);
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n == 0) {
            printf("client finished, leaving\n");
            return;
        }
        if (n < 0) {
            printf("in worker error: %s\n", strerror(errno));
            return ;
        }
        int wrote = 0;
        while (wrote < n) {
            Waiter::wait(fd, POLLOUT);
            ssize_t m = write(fd, buf + wrote, n - wrote);
            if (m < 0) {
                printf("in worker error: %s\n", strerror(errno));
                return ;
            }
            wrote += m;
        }
    }
}

const int PORT = 8021;

int main() {
    Fiber main_fiber([] () {
        std::cout << "main enetered" << endl;
        Fiber global_fiber([]() {
            int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (socket_fd < 0) {
                printf("socket error: %s\n", strerror(errno));
                exit(0);
            }
            int ret = fcntl(socket_fd, F_SETFL, O_NONBLOCK);
            if (ret == -1) {
                printf("fcntl error: %s\n", strerror(errno));
                exit(0);
            }
            sockaddr_in sin{};
            sin.sin_family = AF_INET;
            sin.sin_port = htons(PORT);
            sin.sin_addr = in_addr{0};
            if (bind(socket_fd, reinterpret_cast<const sockaddr *>(&sin), sizeof(sin)) < 0) {
                printf("bind error: %s\n", strerror(errno));
                exit(0);
            }
            if (listen(socket_fd, 10) < 0) {
                printf("listen error: %s\n", strerror(errno));
                exit(0);
            }
            printf("listening on port %d\n", PORT);
            while (true) {
                printf("accepting\n");
                Waiter::wait(socket_fd, POLLIN);
                int client_fd = accept4(socket_fd, nullptr, nullptr, SOCK_NONBLOCK);
                Fiber thread(worker, client_fd);
            }
        });
    });
    startFiberManager();
    main_fiber.join();
}
