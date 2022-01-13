////////미완성!!!!!!!!!!!!!/////////////////////////
/*Socket서버*/

#pragma comment(lib,"Ws2_32.lib")
#include <iostream>
#include <string>
#include <stdio.h>
#include<winSock2.h> //socket의 관련된 함수를 사용하기위한 라이브러리
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
using namespace std;

#define PORT 20
#define clinet_max 10
#define buf_size 200
int client_socket_num[clinet_max];
int clint_num_count = 0;

void client_recv(int cliSocket)
{
	int client_socket = cliSocket;
	char buff[buf_size];
	int recv_len = 0;
	mutex mutex;
	
	while (1)
	{
		/* recv */
		/*
		  * retune 값
		  - 수신 버퍼에 데이터가 도달한 경우
		  : 버퍼에 복사한 후 실제 복사한 바이트 수를 리턴 합니다. 이 경우 recv() 함수의 리턴 값은 최소 1, 최대 len 이 됩니다.

		  - 접속이 정상 종료한 경우
		  : client가 closesocket() 함수를 호출해 접속을 종료하면, TCP 프로토콜 수준에서 접속 종료를 위한 패킷 교환 절차가 일어납니다. 이 경우 recv() 함수는 0을 리턴.
			recv() 함수의 리턴 값이 0인 경우를 정상종료(normal close , graceful close)라고 부릅니다.
		  : 비정상적으로 종료시 recv 함수의 리턴값이 -1로 반환

		   * 인자
		   - 데이터를 받은 클라이언트 소켓
		   - 데이터를 받을 변수
		   - 받는 데이터의 크기(byte)
		   - 옵션
		*/
		recv_len = recv(client_socket, buff, sizeof(buff),0);
		if (recv_len == -1) //client가 비정상적으로 종료
		{
			printf("client(%d) close", client_socket);
			break;
		}
		cout << buff << endl;
	}
}

int main()
{
	WSADATA wasData; //windows용 socket프로그래밍(원속) /윈도우가 아닌 리눅스를 사용하게 되면 write,read와 같이 함수가 다르다
	if (WSAStartup(MAKEWORD(2, 2), &wasData) != 0) //구조체 초기화 ,(,)->윈도우 버전??
	{
		cout << "윈도우 소켓 초기화 실패" << endl;
		return 0;
	}

	/*서버소켓 생성*/
	/* *소켓은 반환이 int형 태이다
	   
	   *인자
	   - AF_INET=PF_INET: TCP IPv4를 사용한다는 의미 
	   - Socket type -> SOCK_STREAM: TCP 프로토콜 통신 방법을 사용 / SOCK_DGRAM: UDP 프로토콜 통신 방법을 사용
	   - "0"을 넣으면 자동적으로 2번째 인자에 맞추어 자동적으로 인자값을 지정 해줌 = IPPROTO_TCP*/
	int  servSocket;
	servSocket = socket(PF_INET, SOCK_STREAM, 0); //int??????Socket???????
	if (servSocket == INVALID_SOCKET)
	{
		cout << "소켓 생성 실패" << endl;
		WSACleanup(); //원속 종료
		return 0;
	}
	else
		cout << "serve 소켓 생성" << servSocket <<endl;

	/*소켓의 기본정보를 구조체로 정의->bind할 정보들*/
	/* - 기본적으로 프로토콜을 사용할때는 PF_INET, address를 지정할때는 AF_INET사용
	   - bind할때 어떤주소로 할것인지 -> INADDR_ANY: 현재의 프로토콜의 ip를 자동으로 가져온다
	   - bind할때 포트     */
	SOCKADDR_IN servAddress;
	servAddress.sin_family = AF_INET; //IPv4를 사용함
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddress.sin_port = htons(PORT);

	//if (inet_pton(AF_INET, "165.229.185.206", &servAddress.sin_addr) != 1) //ip의 주소를 입력하고 해당 문자열 ip를 binary로 바꾼뒤 구조체의 ip주소에 할당
	//{
	//	cout << "inet_pton" << endl;
	//	WSACleanup(); //원속 종료
	//	return 0;
	//}

	/*bind-생성한 소켓에 소켓 기본정보 구조체를 연결한다.*/
	if (bind(servSocket, (SOCKADDR*)&servAddress, sizeof(servAddress)) == -1) //bind가 제대로 되지 않았을시 "-1"
	{
		cout << "bind 실패" << endl;
		closesocket(servSocket);//생선한 socket 반납
		WSACleanup(); //원속 종료
		return 0;
	}
	else
		cout << "bind성공" << endl;

	/*listen- 두번째 인자는 클라이언트 연결 대기열 크기이다, 동시에 여러 클라이언트가 요청이 들어올수 있느니*/
	if (listen(servSocket, 5) == -1)
	{
		cout << "listen 실패 " << endl;
		closesocket(servSocket);//생선한 socket 반납
		WSACleanup(); //원속 종료
		return 0;
	}
	else
		cout << "listen sucess" << endl;

	SOCKADDR_IN clientAddr;
	int cliSocket;
	int clientAddr_len;
	char buff[buf_size];
	int recv_len=0;
	//string str;

	/*accept- */
	while (1)
	{
		/*클라이언트 socket과 데이터를 주고받을 socket생성*/
		/* 
		   *socket은 두종류 
		   1) server소켓으로 clint를 처음 listen할때 사용
		   2) clinet와 실질적으로 데이터를 주고받는 소켓->clint의 개수만큼 accept하여 소켓의 개수가 늘어난다.
		   - clientAddr에 들어온 클라이언트의 소켓 정보(ip등)를 저장한다.
		*/
		clientAddr_len = sizeof(clientAddr);
		cliSocket = accept(servSocket, (SOCKADDR*)&clientAddr, &clientAddr_len);  // int??????Socket???????
		if (cliSocket == INVALID_SOCKET)
		{
			cout << "accept 실패";
			closesocket(servSocket);//생선한 socket 반납
			closesocket(cliSocket);//생선한 socket 반납
			WSACleanup(); //원속 종료
			return 0;
		}
		else
			cout << "clint socket 연결 완료" << cliSocket << endl;

		client_socket_num[clint_num_count++] = cliSocket; //client가 늘어나면 그 수만큼 늘려준다.
		thread thread_client(client_recv, cliSocket);

	}


	WSACleanup(); //원속 종료
	return 0;
}