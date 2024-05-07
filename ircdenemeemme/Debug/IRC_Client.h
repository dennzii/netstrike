#include <future>
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include <winsock2.h> // header file for WinSock 2

#pragma comment(lib,"ws2_32.lib")
using namespace std;
class IRC_Client
{
public:
	
	SOCKET skt;
	WSADATA ws_data;
	sockaddr_in addr;
	string channel;
	int port;
	string ip;

	string nick;
	string username;

	bool IsJoined = false;

	IRC_Client(string IP,int PORT,string Channel,string Nick,string Username)
	{
		ip = IP;
		port = PORT;
		channel = Channel;
		nick = Nick;
		username = Username;

		WSAStartup(MAKEWORD(2, 0), &ws_data);
		skt = socket(AF_INET, SOCK_STREAM, 0);

		if (skt == INVALID_SOCKET)
			cout << "Soket acilamadi";
		else
			cout << "Soket basariyla acildi!" << endl;

		
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		
		
	}
	 int Connect()
	{
		int a = connect(skt, (sockaddr*)&addr, sizeof(addr));
		if (a == SOCKET_ERROR)
		{
			cout << "soket baglanamadi";
			closesocket(skt);
			WSACleanup();
			return 0;
		}
		else
		{
			cout << "soket baglandi";
		}

		SendRaw("PASS secretpasswordhere\r\n");
		SendRaw("NICK " + nick + "\r\n");
		SendRaw("USER " + nick + " * * :" + username + "\r\n");

		while (true)
		{
			ReadRaw();
		}
		return 1;
	}
	void Join()
	{
		SendRaw("JOIN "+channel+"\r\n");
	}
	int SendRaw(string text)
	{
		int sendresult = send(skt, text.c_str(), text.size(), 0);
		if (sendresult != SOCKET_ERROR)
		{
			cout <<"Sent:" << text << endl;
			return 1;
		}
		else
			return 0;
	}

	void ReadRaw()
	{
		char buf[4096];
		ZeroMemory(buf, 4096);
		int bytes_recieved = recv(skt, buf, 4096, 0);
		if (bytes_recieved > 0)
		{
			string message = string(buf, 0, bytes_recieved);
			cout << message << endl;
			if (message.substr(0, 4) == "PING")
			{
				string code = message.substr(6, message.length() - 6);
				SendRaw("PONG :" + code);
			}
			
			if (message.find("353") !=string::npos)
			{
				IsJoined = true;
				cout << "Kanala girildi!";
			}
		}
	}
	~IRC_Client()
	{

	}
};

