#include "ThreadJob.h"
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
	catch (const std::exception& e)
	{
		Rollback();
		DeadlockRecoverThread::GetInstance().InsertJob(std::make_shared<ThreadJob>(*this));
		// Print log
		return;
	}

	Commit();
	isCommitted = true;
}