#include "mutex.h"

void Mutex::lock() {
    while (locked) {
        cv.wait();
    }
    locked = true;
}

void Mutex::unlock() {
    locked = false;
    cv.notify_one();
}
