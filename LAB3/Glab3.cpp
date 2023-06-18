#include <iostream>
#include <windows.h>
using namespace std;

CRITICAL_SECTION criticalSection; 
HANDLE hOutEvent, hMultEvent;
double* finalArray;
int multResult = 1;
int k = 0;
double A = 0;

struct Information {
    int arraySize;
    double* originalArray;
    int k;
    double A;
};


DWORD WINAPI workThread(void* obj)
{
    Information* info = (Information*)obj;
    double* f_arr = info->originalArray;

    cout << "--- workThread is started" << endl;

    int restInterval;
    cout << "Enter time interval to rest after preparing each element: ";
    cin >> restInterval;

    finalArray = new double[info->arraySize];
    int finalArraySize = 0;
    for (int i = 0; i < info->arraySize; i++) {
        if (f_arr[i] < A) {
            finalArray[finalArraySize] = f_arr[i];
            finalArraySize++;
        }
        else {
            f_arr[i - finalArraySize] = f_arr[i];
        }

        Sleep(restInterval);
    }

    info->arraySize = finalArraySize;
    cout << "Final array elements: ";
    for (int i = 0; i < info->arraySize; i++) {
            cout << finalArray[i] << " ";
    }
    cout << endl;
    cout << "--- workThread is finished" << endl << endl;

    SetEvent(hOutEvent);

    return 0;
}

DWORD WINAPI multElementThread(void* obj)
{
    EnterCriticalSection(&criticalSection);
    WaitForSingleObject(hMultEvent, INFINITE);
    cout << "--- multElementThread is started" << endl;

    Information* info = (Information*)obj;

    double multResult = 1;
    for (int i = info->k; i < info->arraySize; i++) {
        multResult *= finalArray[i];
    }

    cout << "Multiplication result: " << multResult << endl;

    LeaveCriticalSection(&criticalSection);

    cout << "--- multElementThread is finished" << endl;
    return 0;
}

int main()
{
    // INPUT DATA
    int arraySize;
    cout << "Enter array size: ";
    cin >> arraySize;

    cout << "Enter array elements: ";
    double* originalArray = new double[arraySize];

    for (int i = 0; i < arraySize; i++) {
        std::cin >> originalArray[i];
    }


    // PRINT INPUT DATA
    cout << "Original array size: " << arraySize << std::endl;

    cout << "Original array elements: ";
    for (int i = 0; i < arraySize; i++) {
        cout << originalArray[i] << " ";
    }
    cout << endl;

    // INFORMATION
    Information* info = new Information();
    info->originalArray = originalArray;
    info->arraySize = arraySize;

    // INITIALIZING
    InitializeCriticalSection(&criticalSection);


    hOutEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    hMultEvent = CreateEvent(NULL, FALSE, FALSE, NULL); 

    HANDLE hThreadWork;
    DWORD IDThreadWork;
    DWORD dwCountWork;
    hThreadWork = CreateThread(NULL, 0, workThread, (void*)info, CREATE_SUSPENDED, &IDThreadWork);
 

    HANDLE hThreadMult;
    DWORD IDThreadMult;
    DWORD dwCountMult;
    hThreadMult = CreateThread(NULL, 0, multElementThread, (void*)info, NULL, &IDThreadMult);

    cout << endl;
    cout << "Enter k: ";
    cin >> k;
    info->k = k;

    cout << "Enter A: ";
    cin >> A;
    info->A = A;
    cout << endl;

    dwCountWork = ResumeThread(hThreadWork);
    WaitForSingleObject(hOutEvent, INFINITE);
  
    cout << "Elements of array: ";
    for (int i = 0; i < info->arraySize; i++) {
        std::cout << finalArray[i] << " ";
    }
    cout << endl;
    cout << endl;

    SetEvent(hMultEvent);
    EnterCriticalSection(&criticalSection); // Критический раздел вводится с помощью EnterCriticalSection(&criticalSection) для защиты общей переменной multResult.

    cout << endl << "Result of MultElement: " << multResult << endl;

    DeleteCriticalSection(&criticalSection); // Выход из критической секции осуществляется с помощью 

    WaitForSingleObject(hThreadWork, INFINITE); // Основной поток ожидает завершения двух потоков
    WaitForSingleObject(hThreadMult, INFINITE);

    CloseHandle(hThreadWork); // Дескрипторы двух потоков закрываются
    CloseHandle(hThreadMult);

    //WaitForSingleObject(hThreadMult, INFINITE);
    //CloseHandle(hThreadMult);
    return 0;
}
