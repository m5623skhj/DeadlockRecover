#include "ThreadJob.h"
#include <iostream>
#include "DeadlockRecoverThread.h"

void ThreadJob::Do()
{
	if (isCommitted)
	{
		return;
	}

	try
	{
		Execute();
	}
	catch (const DeadlockException& e)
	{
		Rollback();
		WriteRollbackLog(e);
		DeadlockRecoverThread::GetInstance().InsertJob(shared_from_this());
		return;
	}

	Commit();
	isCommitted = true;
}

void ThreadJob::WriteRollbackLog(const DeadlockException& e)
{
	std::cout << e.what();
}

std::unique_lock<std::timed_mutex> ThreadJob::AcquireLock(std::timed_mutex& mutex, const std::chrono::milliseconds& timeout, const std::source_location& location)
{
	std::unique_lock lock(mutex, std::defer_lock);
	if (not lock.try_lock_for(timeout))
	{
		throw DeadlockException(location);
	}

	return lock;
}