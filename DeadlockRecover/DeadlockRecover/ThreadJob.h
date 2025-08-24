#pragma once
#include <exception>
#include <mutex>
#include <source_location>

class DeadlockException;

class ThreadJob : public std::enable_shared_from_this<ThreadJob>
{
public:
	virtual ~ThreadJob() = default;

public:
	void Do();

public:
	virtual void Execute() = 0;

	virtual void Commit() = 0;
	virtual void Rollback() = 0;

public:
	static std::unique_lock<std::timed_mutex> AcquireLock(std::timed_mutex& mutex
		, const std::chrono::milliseconds& timeout = std::chrono::milliseconds(500)
		, const std::source_location& location = std::source_location::current());

private:
	static void WriteRollbackLog(const DeadlockException& e);

private:
	bool isCommitted = false;
};

class DeadlockException final : public std::runtime_error
{
public:
	explicit DeadlockException(const std::source_location& location = std::source_location::current());

public:
	[[nodiscard]]
	const char* what() const noexcept override
	{
		const std::string log("Deadlock detected at " + line + '\n');
		return log.c_str();
	}

private:
	std::string line;
};