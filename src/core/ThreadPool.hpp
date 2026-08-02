#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <stdexcept>

namespace Minecraft {

class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            if (m_Stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            m_Tasks.emplace([task]() { (*task)(); });
        }
        m_Condition.notify_one();
        return res;
    }

    size_t getPendingTasks() const;

private:
    std::vector<std::thread> m_Workers;
    std::queue<std::function<void()>> m_Tasks;
    
    mutable std::mutex m_QueueMutex;
    std::condition_variable m_Condition;
    std::atomic<bool> m_Stop{false};
};

}

#endif // THREAD_POOL_HPP
