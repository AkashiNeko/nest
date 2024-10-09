#pragma once

#ifndef NEST_THREAD_POOL_H
#define NEST_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace nest {

class ThreadPool {
public:
    ThreadPool() = default;
    ~ThreadPool();
    
    void run(std::size_t threads);
    void push(std::function<void()>);
    void stop();

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_ = false;

}; // class ThreadPool

} // namespace nest

#endif // NEST_THREAD_POOL_H
