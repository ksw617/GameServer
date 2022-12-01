#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h" // 추가


HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(socket);
}

void Session::Observe(IocpEvent* iocpEvent, int32 bytes)
{
	//이벤트 타입에 따라 진행
	switch (iocpEvent->GetType())
	{
	case IO_TYPE::CONNECT:
		ProcessConnect();
		break;
	case IO_TYPE::RECV:
		ProcessRecv(bytes);
		break;
	default:
		break;
	}
}

void Session::ProcessConnect()
{
	//connected에 true 대입
	//connected = true;
	connected.store(true);

	//서비스에 내 Session 추가
	GetService()->AddSession(GetSession());

	//RegisterRecv 함수 실행
	RegisterRecv();
}

void Session::RegisterSend(SendEvent* sendEvent)
{
}

void Session::RegisterRecv()
{
	//만약에 연결 되어 있지 않다면
	if (IsConnected() == false)
	{
		return;
	}

	//recvEvent 초기화
	recvEvent.Init();

	//recvEvent는 여기 Session에 종속
	//Session을 통해 클라이언트와 데이터를 주고(send), 받고(recv) 하기위해
	recvEvent.owner = shared_from_this();

	//WSARecv 함수 참고
	WSABUF dataBuf;
	dataBuf.buf = reinterpret_cast<char*>(recvBuffer);
	dataBuf.len = sizeof(recvBuffer);

	DWORD recvBytes = 0;
	DWORD flags = 0;

	if (WSARecv(socket, &dataBuf, 1, &recvBytes, &flags, &recvEvent, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = WSAGetLastError();
		//현재 상태가 대기 생태가 아니라면 진짜 에러
		if (errorCode != WSA_IO_PENDING)
		{
			//에러 코드 확인
			HandleError(errorCode);
			//owner는 해제 
			recvEvent.owner = nullptr;
		}

	}

}


void Session::ProcessRecv(int32 bytes)
{
	//받았으니까
	recvEvent.owner = nullptr; // 밀어버리고

	//받은 데이터가 0이면 문제있는거니까
	if (bytes == 0)
	{
		//연결끊고 
		Disconnect(L"Recv Data 0");
		return;
	}

	//얼마 받았는지 확인
	printf("Recv Data Length : %d byte\n", bytes);

	//다시 수신 등록
	RegisterRecv();
}

void Session::Send(BYTE* buffer, int32 len)
{
}

Session::Session()
{
	socket = SocketHelper::CreateSocket();
}

Session::~Session()
{
	SocketHelper::Close(socket);
}

void Session::ProcessSend(SendEvent* sendEvent, int32 bytes)
{
}

void Session::HandleError(int32 error)
{
	switch (error)
	{
		//진짜 에러라면 그냥 연결 끊기
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		//다른거라면 분석
		printf("Handle Error : %d\n", error);
		break;
	}
}

void Session::Disconnect(const WCHAR* cause)
{

	//connected가 true라면 false로 바꿔주고 오래된 값 true를 반환
	//connected가 false라면 오래된 값이 false니까 return 시켜주고 끝냄
	//한번만 실행 시켜주기 위해서
	if (connected.exchange(false) == false)
	{
		return;
	}

	//둘중 아무거나
	//printf("Disconnect : %S", cause);
	printf("Disconnect : %ws", cause);

	//소켓 닫아주고
	SocketHelper::Close(socket);
	//내 session 제거
	GetService()->RemoveSession(GetSession());
}
