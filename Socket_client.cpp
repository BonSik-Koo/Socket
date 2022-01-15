/*Socket 클라이언드-간단한 채팅프로그램 */
#pragma comment(lib,"Ws2_32.lib") 
#include <stdio.h>
#include <iostream>	
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <mutex>
using namespace std;

#define IP ""
mutex mt;

void* recv_data(void* arg)
{
	int sev_socket = (int)arg; //받은 자요형을 변환후 사용
	int recv_len = 0;
	char buff[100] = { NULL };

	while (1)
	{
		recv_len = recv(sev_socket, buff, sizeof(buff), 0);
		if (recv_len == -1 || recv_len == 0)
		{
			cout << "not recive dat" << endl;
			break;
		}
		else
		{
			cout << "recive: "<< buff << endl;
		}
		Sleep(100);
	}
	return NULL;
}

int main()
{
	string input;
	WSADATA wasData; //windows용 socket프로그래밍(원속)
	if (WSAStartup(MAKEWORD(2, 2), &wasData) != 0)
	{
		cout << "윈도우 소켓 초기화 실패" << endl;
		return 0;
	}

	int SeverSocket;
	SeverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (SeverSocket == INVALID_SOCKET)
	{
		cout << "소켓 생성 실패" << endl;
		WSACleanup(); //원속 종료
		return 0;
	}
	else
		cout << "serve 소켓 생성" << endl;

	SOCKADDR_IN Sever_Address;
	/* 
	 -memset
	 :첫번째 인자 void* ptr은 세팅하고자 하는 메모리의 시작 주소.즉, 그 주소를 가리키고 있는 포인터가 위치하는 자리 입니다.
	  두번째 인자 value는 메모리에 세팅하고자 하는 값을 집어 넣으면 됩니다.int 타입으로 받지만 내부에서는 unsigned char 로 변환되어서 저장됩니다. 즉 'a' 이런것을 넣어도 무방하다는 뜻입니다.
	  세번째 인자 size_t num은 길이를 뜻합니다. 이 길이는 바이트 단위로써 메모리의 크기 한조각 단위의 길이를 말합니다. 이는 보통 "길이 * sizeof(데이터타입)" 의 형태로 작성하면 됩니다.
	*/
	//memset(&Sever_Address, 0, sizeof(SOCKADDR_IN)); 
	Sever_Address.sin_family = AF_INET;
	Sever_Address.sin_addr.s_addr = inet_addr(IP);
	Sever_Address.sin_port = htons(20);

	if (connect(SeverSocket, (SOCKADDR*)&Sever_Address, sizeof(Sever_Address)) == -1)
	{
		cout << "connect arror" << endl;
		closesocket(SeverSocket);
		WSACleanup(); //원속 종료
		return 0;
	}
	else
		cout << "connect suessess" << endl;

	thread recv_thread(recv_data, (void*)SeverSocket); //void* :자료형을 지정해주지 않는다는 의미이다
	recv_thread.detach();

	while (1)
	{
		cout << "input 입력" << endl;
		getline(cin,input);
		send(SeverSocket, input.c_str(), input.length(), 0);
		cout << "send: " << input << endl;
	
		Sleep(100);
	}

	closesocket(SeverSocket);
	WSACleanup(); //원속 종료

	return 0;
}
