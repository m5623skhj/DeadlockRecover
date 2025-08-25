#pragma once
#include <mutex>
#include <source_location>

class DeadlockException;
class DeadlockRecoverThread;

class ThreadJob : public std::enable_shared_from_this<ThreadJob>
{
	friend DeadlockRecoverThread;

public:
	virtual ~ThreadJob() = default;

public:
	void Do();

public:
	virtual void Execute() = 0;

	virtual void Commit() = 0;
	virtual void Rollback() = 0;

public:
	std::unique_lock<std::timed_mutex> AcquireLock(std::timed_mutex& mutex
		, const std::chrono::milliseconds& timeout = std::chrono::milliseconds(500)
		, const std::source_location& location = std::source_location::current()) const;

private:
	static void WriteRollbackLog(const DeadlockException& e);
	void SetNeedNonTimerLock(bool need);
	static std::unique_lock<std::timed_mutex> AcquireNonTimerLock(std::timed_mutex& mutex);

private:
	bool needNonTimerLock = false;
	bool isCommitted = false;
};

class DeadlockException final : public std::runtime_error
{
public:
	explicit DeadlockException(const std::source_location& location = std::source_location::current());
};