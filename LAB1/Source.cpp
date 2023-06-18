#include <windows.h>
#include <iostream>
#include <ctime>
using namespace std;


struct Data {
	double* masOfInput;
	int sizeInput;
	double* masOfOutput;
	int sizeOutput;
};

DWORD WINAPI worker(LPVOID mas)
{
	auto* input = (Data*)mas;
	input->masOfOutput = new double[input->sizeInput];
	input->sizeOutput = 0;
	double X;
	cout << "Enter X, a number, the integer part of which we will compare:" << endl;
	cin >> X;
	for (int i = 0; i <  input->sizeInput; i++) {
		if (floor(input->masOfInput[i]) == floor(X)) {
			input->sizeOutput++;;
		}
	}	
	
	return 0;
}


int main()
{
	double X;
	Data* input = new Data;
	input->masOfInput = new double[100];
	cout << "Enter size of your array: ";
	cin >> input->sizeInput;
	for (int i = 0; i < input->sizeInput; i++) {
		cout << "Enter the element of your array:" << endl;
		cin >> X;
		input->masOfInput[i] = X;
	}

	for (int i = 0; i < input->sizeInput; i++) {
	
		cout << input->masOfInput[i] << " ";
	}

	cout << endl;

	int stopTime;
	cout << "Enter stop time for worker(in seconds): ";
	cin >> stopTime;

	HANDLE	hHandle;
	DWORD	IDThread;
	hHandle = CreateThread(NULL, 0, worker, input, 0, &IDThread);
	if (hHandle == NULL) {
		return GetLastError();
	}

	SuspendThread(hHandle);
	Sleep(1000 * stopTime);
	ResumeThread(hHandle);

	WaitForSingleObject(hHandle, INFINITE);
	CloseHandle(hHandle);
	cout << "Amount of numbers with the same integer parts equals: " << input->sizeOutput << endl;


	/*hThread = (HANDLE)
				_beginthreadex(NULL, 0, thread, sentence, 0, &IDThread);
			if (hThread == NULL)
				return GetLastError();*/

	delete input;
	return 0;
}