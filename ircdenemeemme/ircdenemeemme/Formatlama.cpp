#include <future>
#include <iostream>
#include <string>
#include "Formatlama.h"
#include <string>

using namespace std;

string TrimSpace(string text)
{
	string TrimmedString;
	int lastCha;
	for (int i = 0;i <= text.size();i++)
	{
		if (text[i] != ' ')
			lastCha = i;
	}
	TrimmedString = text.substr(0, lastCha);
	return TrimmedString;
}

int ilkKarakteriBul(string text)
{
	for (int i = 0;i <= text.size();i++)
	{
		if (text[i] == ' ')
			return i;
	}
	return 0;
}




