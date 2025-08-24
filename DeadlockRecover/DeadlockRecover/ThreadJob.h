#pragma once

class ThreadJob
{
public:
	virtual ~ThreadJob() = default;

public:
	virtual void Execute() = 0;
};