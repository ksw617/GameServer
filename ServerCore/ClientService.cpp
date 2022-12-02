#include "pch.h"
#include "ClientService.h"
#include "Session.h" // 추가

bool ClientService::Start()
{
	//sessionFactory가 null이라면
	if (sessionFactory == nullptr)
	{
		//실패
		return false;

	}
									  
	//sessionFactory에서 뱉어내는 session 받아서
	shared_ptr<Session> session = CreateSession();
	//연결 호출
	//만약에 결과값이 false라면
	if (session->Connect() == false)
	{
		//실패
		return false;
	}

    return true;
}
