#include "DeadlockRecoverThread.h"

#include <functional>

#include "ThreadJob.h"

void DeadlockRecoverThread::Start()
{
	if (isRunning == true)
	{
		return;
	}

	recoverThread = std::jthread([this](const std::stop_token& stopToken) { Run(stopToken); });
	isRunning = true;
}

void DeadlockRecoverThread::Stop()
{
	if (isRunning == false)
	{
		return;
	}

	recoverThread.request_stop();
	recoverThread.join();
	isRunning = false;
}

void DeadlockRecoverThread::Run(const std::stop_token& stopToken)
{
	while (stopToken.stop_requested() == false)
	{
		recoverThreadJobSemaphore.acquire();

		std::shared_ptr<ThreadJob> job;
		{
			std::unique_lock lock(jobQueueMutex);
			if (jobQueue.empty())
			{
				continue;
			}

			job = jobQueue.front();
			jobQueue.pop();
		}

		if (job != nullptr)
		{
			job->Do();
		}
	}
}

void DeadlockRecoverThread::InsertJob(std::shared_ptr<ThreadJob>&& job)
{
	{
		std::unique_lock lock(jobQueueMutex);
		jobQueue.push(std::move(job));
	}

	recoverThreadJobSemaphore.release();
}
