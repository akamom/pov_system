#include <iostream>
#include <winsock2.h>
#include <ws2bth.h>
SOCKADDR_BTH sockAddr;
SOCKET btSocket;
int error;

int main()
{
	//unsigned char* p;
	//ULONGLONG ullAddr;

	//p = (unsigned char*)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr; // point to addr
	//memcpy(&ullAddr, &p[2], 8); // copy Bluetooth address of device we found
	
	btSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.addressFamily = AF_BTH;
	sockAddr.serviceClassId = RFCOMM_PROTOCOL_UUID;
	sockAddr.port = BT_PORT_ANY;
	sockAddr.btAddr = (ULONGLONG)"E4A7C5E77AEB";
	error = connect(btSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	std::cout << error << std::endl;
}



#include <windows.h>

int main() {
	// Open serial port
	HANDLE serialHandle;

	serialHandle = CreateFile(L"\\\\.\\COM6", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	// Do some basic settings
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = 9600;
	serialParams.ByteSize = 8;
	serialParams.StopBits = TWOSTOPBITS;
	serialParams.Parity = NOPARITY;
	SetCommState(serialHandle, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;

	SetCommTimeouts(serialHandle, &timeout);

	std::string data = "test text";
	DWORD bytesrecieved;
	WriteFile(serialHandle, &data, data.length(), &bytesrecieved, NULL);
	std::cout << bytesrecieved << std::endl;
	std::cout << serialHandle << std::endl;

	CloseHandle(serialHandle);
}