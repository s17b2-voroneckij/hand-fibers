#include "condvar.h"

class Mutex {
public:
    void lock();
    void unlock();

private:

    CondVar cv;
    bool locked = false;
};
