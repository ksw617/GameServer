#pragma once


class Listener
{
private:
	SOCKET socket = INVALID_SOCKET;
	//HANDLE iocpHandle = NULL;
public:
	Listener() = default;
	~Listener();
public:
	//HANDLE GetHandle() const { return iocpHandle; }
public:
	//�����ͷ� ����
	bool StartAccept(class Service* service);
	void CloseSocket();
	
};

