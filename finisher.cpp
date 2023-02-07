#include "finisher.hpp"
#include <atomic>

volatile std::atomic<bool> Finisher::finish = false;