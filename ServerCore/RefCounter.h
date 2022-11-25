#pragma once
class RefCounter
{
protected:
	atomic<int32> refCount;
public:
	RefCounter();
	virtual ~RefCounter() {};
public:
	int32 GetRefCount();
	int32 AddRef();
	int32 RemoveRef();
};

