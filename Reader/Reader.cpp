#define _CRT_SECURE_NO_WARNINGS //so that there are no warnings when using functions for working with files from the "C" language
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <sysinfoapi.h>
#include <string>
#include <time.h>
//3+5+2=10
#define PageNum 10
using namespace std;
HANDLE* WrSem = new HANDLE[PageNum]; //Semaphore for memory pages
HANDLE* RdSem = new HANDLE[PageNum];
void WSem(int NumSem);//Function to open writer's semaphore
void RSem(int NumSem);//Function to open reader's semaphore
void OpenSem();
int BufSizeCounter();//Function to count buffer memory
int main()
{
	srand(time(NULL));
	int BufferSize = BufSizeCounter();
	string nameBuffer, nameMap;
	HANDLE HandleMapped;
	LPVOID address;
	nameBuffer = "C:\\Users\\vova_\\source\\repos\\oslab4\\buffer.txt";
	nameMap = "buffer";
	HandleMapped = OpenFileMappingA(GENERIC_WRITE, FALSE, nameMap.c_str());
	address = MapViewOfFile(HandleMapped, FILE_MAP_WRITE, 0, 0, BufferSize);
	FILE* file1, * file2;
	string nameLogfile;
	nameLogfile = "C:\\Users\\vova_\\source\\repos\\oslab4\\ReaderLogs\\reader_log_file_" + to_string(GetCurrentProcessId()) + ".txt";
	file1 = fopen(nameLogfile.c_str(), "w");
	string nameLogfileExcel;
	nameLogfileExcel = "C:\\Users\\vova_\\source\\repos\\oslab4\\ReaderExcel\\reader_log_file_" + to_string(GetCurrentProcessId()) + ".txt";
	file2 = fopen(nameLogfileExcel.c_str(), "w");
	OpenSem();
	char* data;
	SYSTEM_INFO systemInf;
	GetSystemInfo(&systemInf);
	DWORD start = GetTickCount();
	while (GetTickCount64() < start + 15000)
	{
		fprintf(file1, "Process: %d\t  State: WAITING  \t\t  Time: %d  \n", GetCurrentProcessId(), GetTickCount());
		fprintf(file2, "%d %d\n", GetTickCount(), 0);
		DWORD pageSemOne = WaitForMultipleObjects(PageNum, RdSem, FALSE, INFINITE); //Waiting for the semaphore page to get the value 1
		fprintf(file1, "Process: %d\t  State: READING page %d\t  Time: %d  \n", GetCurrentProcessId(), pageSemOne, GetTickCount());
		fprintf(file2, "%d %d\n", GetTickCount(), 1);
		data = (char*)((long long)address + (pageSemOne * systemInf.dwPageSize));
		Sleep(500 + rand() % 1001);
		string logname = "C:\\Users\\vova_\\source\\repos\\oslab4\\PageExcel\\pagesemone_" + to_string(pageSemOne) + ".txt";
		FILE* logs = fopen(logname.c_str(), "a");
		fprintf(logs, "%d %d\n", GetTickCount(), 2);
		ReleaseSemaphore(WrSem[pageSemOne], 1, NULL);
		fprintf(logs, "%d %d\n", GetTickCount(), 0);
		fclose(logs);
		fprintf(file1, "Process: %d\t  State: RELEASED Data: %s  \t\t  Time: %d  \n\n", GetCurrentProcessId(), data, GetTickCount());
		fprintf(file2, "%d %d\n", GetTickCount(), 2);
	}
	return 0;
}
void WSem(int NumSem) {
	string nameSem;
	nameSem = to_string(NumSem) + " Writing a semaphore";
	WrSem[NumSem] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, nameSem.c_str());
}
void RSem(int NumSem) {
	string nameSem;
	nameSem = to_string(NumSem) + " Reading a semaphore";
	RdSem[NumSem] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, nameSem.c_str());
}
void OpenSem() {
	for (int i = 0; i < PageNum; i++) {
		WSem(i);
		RSem(i);
	}
}
int BufSizeCounter() {
	int buffer;
	SYSTEM_INFO systemInf;
	GetSystemInfo(&systemInf);
	buffer = PageNum * systemInf.dwPageSize;
	return buffer;
}