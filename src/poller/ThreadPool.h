#pragma once

#ifndef NEST_THREAD_POOL_H
#define NEST_THREAD_POOL_H

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace nest {

class ThreadPool {
public:
    ThreadPool(size_t);
    ~ThreadPool();
    
    void push(std::function<void()>);
    void stop();

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;

};

} // namespace nest

#endif // NEST_THREAD_POOL_H
