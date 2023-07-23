#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <functional>
#include <future>
#include <vector>
#include <condition_variable>

class SimpleThreadPool
{
public:
	explicit SimpleThreadPool(std::size_t threadCount = 1) : mThreadCount(threadCount), stop(false), working(true)
	{
		AddThread(threadCount);
	}

	~SimpleThreadPool()
	{

		if (!threads.empty())
		{
			Destroy();
		}

	}

	SimpleThreadPool(const SimpleThreadPool&) = delete;
	SimpleThreadPool& operator=(const SimpleThreadPool&) = delete;
	SimpleThreadPool(SimpleThreadPool&&) = delete;
	SimpleThreadPool& operator=(SimpleThreadPool&&) = delete;

	template <typename Fnc_T>
	auto Post(Fnc_T task) -> std::future<decltype(task())>
	{
		auto packagedTask = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));
		std::future<decltype(task())> result = packagedTask->get_future();

		{
			std::lock_guard<std::mutex> lock(mut);
			tasks.emplace([packagedTask]()
				{
					try
					{
						(*packagedTask)();
					}
					catch (const std::exception& ex)
					{
						throw ex;
					}
					catch (...)
					{
						throw;
					}
				});
		}

		condition.notify_one();
		return result;
	}

	void WorkOn()
	{
		{
			std::lock_guard<std::mutex> lock(mut);
			stop = false;
			working = true;
		}
		condition.notify_all();
	}

	void WorkOff()
	{
		{
			std::lock_guard<std::mutex> lock(mut);
			stop = true;
			working = false;

		}
		condition.notify_all();
	}

	void ClearQueue()
	{
		std::lock_guard<std::mutex> lock(mut);
		while (!tasks.empty())
		{
			tasks.pop();
		}
	}

	void Destroy()
	{
		ClearQueue();

		{
			std::lock_guard<std::mutex> lock(mut);
			stop = true;
			working = false;
		}

		condition.notify_all();

		for (std::thread& thread : threads)
		{
			if (thread.joinable())
				thread.join();
		}
		threads.clear();
		mThreadCount = 0;
	}

	void AddThread()
	{
		threads.emplace_back([this]()
			{
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(mut);
						condition.wait(lock, [this]()
							{
								return stop || (!tasks.empty() && working);
							});

						if (stop && tasks.empty())
							return;

						task = std::move(tasks.front());
						tasks.pop();
					}

					try
					{
						taskInWork++;
						task();

						taskInWork--;
						completedTasks++;
					}
					catch (const std::exception& ex)
					{
						throw ex;
					}
					catch (...)
					{
						throw;
					}

				}
			});
	}

	void AddThread(std::size_t amountOfThread)
	{
		for (std::size_t i = 0; i < amountOfThread; ++i)
		{
			AddThread();
		}

	}

	std::size_t GetTaskCount()
	{
		std::lock_guard<std::mutex> lock(mut);
		return tasks.size();
	}
	std::size_t GettaskInWorkCount()
	{
		std::lock_guard<std::mutex> lock(mut);
		return taskInWork;
	}

	std::size_t GetThreadCount()
	{
		return threads.size();
	}

	std::size_t GetCompletedWorkItemCount()
	{
		std::lock_guard<std::mutex> lock(mut);
		return completedTasks;
	}

private:
	std::size_t mThreadCount;
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::condition_variable condition;
	std::mutex mut;
	bool stop;
	bool working;
	std::atomic<std::size_t> completedTasks{ 0 };
	std::size_t taskInWork{ 0 };
};


