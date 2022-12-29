#include <windows.h>
#include <iostream>
#include <string>
using namespace std;
int main()
{
	HANDLE pipe;
	string pipe_name, event_name;
	char* data = new char[MAX_PATH];
	OVERLAPPED Overlapped;
	DWORD written;
	ZeroMemory(&Overlapped, sizeof(Overlapped));
	event_name = "EVENT";
	Overlapped.hEvent = CreateEventA(NULL, TRUE, FALSE, event_name.c_str());
	pipe_name = "\\\\.\\pipe\\mypipe";
	pipe = CreateNamedPipeA(pipe_name.c_str(),
				PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, //For reading and writing
				PIPE_TYPE_BYTE | PIPE_WAIT, //Byte-oriented channel will operate in blocking mode when the process is placed in a wait state until the operations on the channel are completed.
				1, //Maximum number of channel implementations
				512, 512, //Size of output/input buffer in bytes
				0, //Timeout in milliseconds
				NULL); //Variable address for security attributes
	ConnectNamedPipe(pipe, &Overlapped);
	cout << "You can input data. Input \"0\" to stop.\n\n";
	while (true)
	{
		cin >> data;
		if (strcmp(data, "0") != 0)
		{
			cout << endl;
			WriteFileEx(pipe, data, strlen(data) + 1, &written, &Overlapped);
			WaitForSingleObject(Overlapped.hEvent, INFINITE);
		}
		else
		{
			cout << "\nServer stopped\n";
			break;
		}
	}
}
