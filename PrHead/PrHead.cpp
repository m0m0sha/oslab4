#include <windows.h>
#include <iostream>
#include <sysinfoapi.h>
#include <string>
//3+5+2=10
#define PageNum 10
using namespace std;
void WrCreate(int);//Function to create writer semaphores (initially set to 1)
void RdCreate(int);//Function to create reader semaphores (initially set to 0)
void ProcWr(int);//Function to create writer processes
void ProcRd(int);//Function to create reader processes
int BufSize();//Function for counting buffer memory
void CreateSem();
void CreateProcess();
void CreateMappingFile(int);
HANDLE* WrSem = new HANDLE[PageNum];//Semaphore for memory pages
HANDLE* RdSem = new HANDLE[PageNum];
HANDLE* startProc = new HANDLE[PageNum << 1];
int main()
{
	int bufSize = BufSize();
	CreateMappingFile(bufSize);//Create projected file
	CreateSem();//Create semaphores and processes
	CreateProcess();
	WaitForMultipleObjects(PageNum << 1, startProc, TRUE, INFINITE);//Waiting for processes to finish
	return 0;
}
void WrCreate(int NumSem){
	string NameSem;
	NameSem = to_string(NumSem) + " Writing a semaphore";
	WrSem[NumSem] = CreateSemaphoreA(NULL, 1, 1, NameSem.c_str());
}
void RdCreate(int NumSem){
	string NameSem;
	NameSem = to_string(NumSem) + " Reading a semaphore";
	RdSem[NumSem] = CreateSemaphoreA(NULL, 0, 1, NameSem.c_str());
}
void ProcWr(int NumProc){
	LPSTARTUPINFOA startup = new STARTUPINFOA[PageNum];
	LPPROCESS_INFORMATION procInf = new PROCESS_INFORMATION[PageNum];
	string NameProc;
	NameProc = "C:\\Users\\vova_\\source\\repos\\oslab4\\Writer\\x64\\Debug\\Writer.exe";
	ZeroMemory(&startup[NumProc], sizeof(startup[NumProc]));//Resetting the memory block
	CreateProcessA(NameProc.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startup[NumProc], &procInf[NumProc]);//Create a process
	startProc[NumProc] = procInf[NumProc].hProcess;//Add writer handle
}
void ProcRd(int NumProc){
	LPSTARTUPINFOA startup = new STARTUPINFOA[PageNum];
	LPPROCESS_INFORMATION procInf = new PROCESS_INFORMATION[PageNum];
	string NameProc;
	NameProc = "C:\\Users\\vova_\\source\\repos\\oslab4\\Reader\\x64\\Debug\\Reader.exe";
	ZeroMemory(&startup[NumProc], sizeof(startup[NumProc]));//Resetting the memory block
	CreateProcessA(NameProc.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startup[NumProc], &procInf[NumProc]);//Create a process
	startProc[PageNum + NumProc] = procInf[NumProc].hProcess;//Add reader handle
}
int BufSize(){
	int buffer;
	SYSTEM_INFO systemInf;
	GetSystemInfo(&systemInf);
	buffer = PageNum * systemInf.dwPageSize;
	return buffer;
}
void CreateSem() {
	for (int i = 0; i < PageNum; i++) {
		WrCreate(i);
		RdCreate(i);
	}
}
void CreateProcess() {
	for (int i = 0; i < PageNum; i++) {
		ProcWr(i);
		ProcRd(i);
	}
}
void CreateMappingFile(int BufferSize) {
	string nameBuffer, nameMap;
	HANDLE handleBuffer, handleMapped;
	LPVOID address;
	nameBuffer = "C:\\Users\\vova_\\source\\repos\\oslab4\\buffer.txt";
	nameMap = "buffer";
	handleBuffer = CreateFileA(nameBuffer.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	handleMapped = CreateFileMappingA(handleBuffer, NULL, PAGE_READWRITE, 0, BufferSize, nameMap.c_str());
	address = MapViewOfFile(handleMapped, FILE_MAP_WRITE, 0, 0, BufferSize);
	VirtualLock(address, BufferSize);
}