#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <semaphore>

class ThreadJob;

class DeadlockRecoverThread
{
private:
	DeadlockRecoverThread() = default;
	~DeadlockRecoverThread() = default;

public:
	[[nodiscard]]
	static DeadlockRecoverThread& GetInstance()
	{
		static DeadlockRecoverThread instance;
		return instance;
	}

	DeadlockRecoverThread(const DeadlockRecoverThread&) = delete;
	DeadlockRecoverThread& operator=(const DeadlockRecoverThread&) = delete;

public:
	void Start();
	void Stop();

	[[nodiscard]]
	bool IsRunning() const { return isRunning; }
	void InsertJob(std::shared_ptr<ThreadJob>&& job);

private:
	void Run(const std::stop_token& stopToken);

private:
	bool isRunning = false;
	std::jthread recoverThread;
	
	std::mutex jobQueueMutex;
	std::queue<std::shared_ptr<ThreadJob>> jobQueue;
	std::counting_semaphore<> recoverThreadJobSemaphore{ 0 };
};