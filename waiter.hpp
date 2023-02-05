#include <unordered_map>

#include "fiber_impl.h"
#include "fiber_manager.h"
#include "condvar.h"

using std::unordered_map;

struct FdRequest {
    CondVar cv;
    int fd;
    short events;
};

class Waiter {
public:
    Waiter();

    static int wait(int fd, short events);

    static void stop();

    static void loop();

private:

    CondVar cv;
    unordered_map<int, FdRequest*> map;
    bool stopped = false;
};