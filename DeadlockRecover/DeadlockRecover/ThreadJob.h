#pragma once
#include <exception>

class ThreadJob
{
public:
	virtual ~ThreadJob() = default;

public:
	void Do();

public:
	virtual void Execute() = 0;

	virtual void Commit() = 0;
	virtual void Rollback() = 0;

private:
	bool isCommitted = false;
};

class DeadlockException : public std::exception
{
public:
	const char* what() const noexcept override
	{
		return "Deadlock detected";
	}
};