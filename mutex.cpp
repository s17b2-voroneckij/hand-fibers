#include "mutex.h"

void Mutex::lock() {
    while (locked.exchange(true)) {
        cv.wait();
    }
    locked = true;
}

void Mutex::unlock() {
    locked = false;
    cv.notify_one();
}
