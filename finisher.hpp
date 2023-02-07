#include <atomic>

class Finisher {
public:
    static volatile std::atomic<bool> finish;
};
