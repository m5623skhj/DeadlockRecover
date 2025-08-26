#include "DeadlockRecoverThread.h"
#include "ThreadJob.h"
#include <iostream>
#include <Windows.h>
#include <mutex>

std::timed_mutex mtx1;
std::timed_mutex mtx2;

class TestThreadJob1 : public ThreadJob
{
public:
	virtual ~TestThreadJob1() override = default;

public:
	void Execute() override
	{
		auto m1 = AcquireLock(mtx1);
		auto m2 = AcquireLock(mtx2);
	}
	void Commit() override
	{
		std::cout << "Thread ID: " << std::this_thread::get_id() << " is committing." << '\n';
	}
	void Rollback() override
	{
		std::cout << "Thread ID: " << std::this_thread::get_id() << " is rolling back." << '\n';
	}
};

class TestThreadJob2 : public ThreadJob
{
public:
	virtual ~TestThreadJob2() override = default;

public:
	void Execute() override
	{
		auto m2 = AcquireLock(mtx2);
		auto m1 = AcquireLock(mtx1);
	}
	void Commit() override
	{
		std::cout << "Thread ID: " << std::this_thread::get_id() << " is committing." << '\n';
	}
	void Rollback() override
	{
		std::cout << "Thread ID: " << std::this_thread::get_id() << " is rolling back." << '\n';
	}
};

int main()
{
	DeadlockRecoverThread::GetInstance().Start();
	std::cout << "Deadlock recover thread started." << '\n';

	std::atomic_bool breaker = false;

	auto t1 = std::jthread([&breaker]()
	{
		std::cout << "Thread ID: " << std::this_thread::get_id() << " is running." << '\n';
		while (breaker.load(std::memory_order_relaxed) == false)
		{
			const auto job = std::make_shared<TestThreadJob1>();
			job->Do();
		}

		std::cout << "Thread ID: " << std::this_thread::get_id() << " is stopping." << '\n';
	});

	auto t2 = std::jthread([&breaker]()
	{
		std::cout << "Thread ID: " << std::this_thread::get_id() << " is running." << '\n';
		while (breaker.load(std::memory_order_relaxed) == false)
		{
			const auto job = std::make_shared<TestThreadJob2>();
			job->Do();
		}
		std::cout << "Thread ID: " << std::this_thread::get_id() << " is stopping." << '\n';
	});

	while (breaker == false)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			breaker.store(true, std::memory_order_relaxed);
		}
	}

	DeadlockRecoverThread::GetInstance().Stop();
	std::cout << "Deadlock recover thread stopped." << '\n';

	Sleep(5000);

	return 0;
}