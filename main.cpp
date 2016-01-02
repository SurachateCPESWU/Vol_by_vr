#include "stdafx.h"
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <strsafe.h>
#include <Wscapi.h>

HRESULT hr;

void set_bass(IMMDevice *defaultDevice,float set)
{
	IDeviceTopology *endpointTopology = NULL;
	defaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_ALL, NULL, (LPVOID *)&endpointTopology);
	IConnector *endpointConnector = NULL;
	IConnector *hardwareConnector = NULL;
	endpointTopology->GetConnector(0, &endpointConnector);
	endpointConnector->GetConnectedTo(&hardwareConnector);
	IPart *hcPart = NULL;
	hardwareConnector->QueryInterface(__uuidof(IPart), (LPVOID *)&hcPart);
	IDeviceTopology *hardwareTopology = NULL;
	hcPart->GetTopologyObject(&hardwareTopology);
	UINT count = NULL;
	hardwareTopology->GetSubunitCount(&count);
	float x = NULL;

	for (int i = 0; i < count; ++i) {
		ISubunit *sub = NULL;
		IPart* hsPart = NULL;
		IAudioBass *bass = NULL;
		hardwareTopology->GetSubunit(i, &sub);
		sub->QueryInterface(__uuidof(IPart), (void**)&hsPart);
		hsPart->Activate(CLSCTX_ALL, __uuidof(IAudioBass), (void**)&bass);
		if (bass != NULL) {
			bass->SetLevel(0, set, NULL);
		}
		sub->Release();
		hsPart->Release();
	}

	endpointTopology->Release();
	endpointConnector->Release();
	hardwareConnector->Release();
	hcPart->Release();
	hardwareTopology->Release();
}

void set_tre(IMMDevice *defaultDevice,float set)
{
	IDeviceTopology *endpointTopology = NULL;
	defaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_ALL, NULL, (LPVOID *)&endpointTopology);
	IConnector *endpointConnector = NULL;
	IConnector *hardwareConnector = NULL;
	endpointTopology->GetConnector(0, &endpointConnector);
	endpointConnector->GetConnectedTo(&hardwareConnector);
	IPart *hcPart = NULL;
	hardwareConnector->QueryInterface(__uuidof(IPart), (LPVOID *)&hcPart);
	IDeviceTopology *hardwareTopology = NULL;
	hcPart->GetTopologyObject(&hardwareTopology);
	UINT count = NULL;
	hardwareTopology->GetSubunitCount(&count);
	float x = NULL;

	for (int i = 0; i < count; ++i) {
		ISubunit *sub = NULL;
		IPart* hsPart = NULL;
		IAudioTreble *Treble = NULL;
		hardwareTopology->GetSubunit(i, &sub);
		sub->QueryInterface(__uuidof(IPart), (void**)&hsPart);
		hsPart->Activate(CLSCTX_ALL, __uuidof(IAudioTreble), (void**)&Treble);
		if (Treble != NULL) {
			Treble->SetLevel(0, set, NULL);
		}
		sub->Release();
		hsPart->Release();
	}
	endpointTopology->Release();
	endpointConnector->Release();
	hardwareConnector->Release();
	hcPart->Release();
	hardwareTopology->Release();
}




void set_volume(IMMDevice *defaultDevice, float set)
{
	IAudioEndpointVolume *endpointVolume = NULL;
	defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	endpointVolume->SetMasterVolumeLevelScalar(set, NULL);
	endpointVolume->Release();
}



void PrintCommState(DCB dcb)
{
	//  Print some of the DCB structure values
	_tprintf(TEXT("\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n"),
		dcb.BaudRate,
		dcb.ByteSize,
		dcb.Parity,
		dcb.StopBits);
}

int _tmain(int argc, _TCHAR* argv[])
{
	float set_vol = 0;
	DCB dcb;
	HANDLE hCom;
	BOOL fSuccess;
	char output[11];
	int isRead = false;
	DWORD dwBytesRead;
	const char* pcCommPort = "COM3";

	hCom = CreateFile(pcCommPort,
		GENERIC_READ | GENERIC_WRITE,
		0,    // must be opened with exclusive-access
		NULL, // no security attributes
		OPEN_EXISTING, // must use OPEN_EXISTING
		0,    // not overlapped I/O
		NULL  // hTemplate must be NULL for comm devices
		);

	fSuccess = GetCommState(hCom, &dcb);
	dcb.BaudRate = CBR_9600;     // set the baud rate
	dcb.ByteSize = 8;             // data size, xmit, and rcv
	dcb.Parity = NOPARITY;        // no parity bit
	dcb.StopBits = ONESTOPBIT;    // one stop bit

	fSuccess = SetCommState(hCom, &dcb);

	if (fSuccess) printf("CONNECT = ");


	PrintCommState(dcb);

	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;;
	HANDLE      handle;

	DWORD oldMode;
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &oldMode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), oldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));


	//################## SETUP #############################
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;
	deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;
	//#####################################################


	char inputChar = '\0';
	int temp = 0;
	char to_int[3] ;
	char type[5] ;
	while (inputChar != '\r')
	{
		for (int i = 0; i < 11;i++) output[i] = NULL;
		isRead = ReadFile(hCom, output, sizeof(output), &dwBytesRead, NULL);
		for (int i = 0; i < 5; i++) type[i] = output[i];
		if (strcmp(type, "s_vol") == 1){
			int j = 0;
			for (int i = 6; i < 10; i++) to_int[j++] = output[i];
			temp = atoi(to_int);
			printf("%d \n", temp);
			set_vol = (float)(temp) / (float)(1000);
			set_volume(defaultDevice, set_vol);
		}
	}

	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), oldMode);

	CoUninitialize();
	return 0;

}
