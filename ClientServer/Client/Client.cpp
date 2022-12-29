#include <windows.h>
#include <iostream>
#include <string>
using namespace std;
void CALLBACK CompletionRoutine(DWORD errorCode, DWORD bytestransfered, LPOVERLAPPED lpOverlapped){}
int main()
{
	HANDLE pipe;
	string pipe_name;
	char* data = new char[MAX_PATH];
	OVERLAPPED Overlapped;
	ZeroMemory(&Overlapped, sizeof(Overlapped));
	pipe_name = "\\\\.\\pipe\\mypipe";
	pipe = CreateFileA(pipe_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		cout << "Error " << GetLastError() << ".\n";
		SetLastError(0);
	}
	cout << "Data from server:\n\n";
	while (true)
	{
		ReadFileEx(pipe, data, 512, &Overlapped, CompletionRoutine);
		SleepEx(INFINITE, TRUE);
		cout << data << endl << endl;
	}
	return 0;
}
