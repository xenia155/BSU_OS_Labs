#include <windows.h>
#include <iostream>
#include <string>
#pragma comment(lib, "kernel32.lib")

using namespace std;

int main(int argc, char* argv[])
{
    HANDLE hMutex;
    HANDLE AEvent;
    HANDLE BEvent;
    HANDLE WriteEnd;
    cout << "Writer " << atoi(argv[1]) << endl;
    //Получаем мьютекс
    hMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"Mutex");
    if (hMutex == NULL)
    {
        DWORD dwLastError = GetLastError();
        cout << "OpenMutex failed with error " << dwLastError << endl;
        cout << "Press any key to exit." << endl;
        cin.get();
        return dwLastError;
    }

    //Получаем события А В и конечное событие
    std::wstring name = L"AEvent_";
    name += to_wstring(atoi(argv[1]));
    auto* wstr_e = new wchar_t[name.length()];
    wcscpy_s(wstr_e, name.length() + 1, name.c_str());

    AEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, wstr_e);
    if (AEvent == nullptr)
    {
        cout << "Open a event failed." << endl;
        cout << "Input any char to exit." << endl;
        cin.get();
        return GetLastError();
    }

    name = L"BEvent_";
    name += to_wstring(atoi(argv[1]));
    wstr_e = new wchar_t[name.length()];
    wcscpy_s(wstr_e, name.length() + 1, name.c_str());
    BEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, wstr_e);
    if (AEvent == nullptr)
    {
        cout << "Open b event failed." << endl;
        cout << "Input any char to exit." << endl;
        cin.get();
        return GetLastError();
    }


    name = L"WriteEndEvent_";
    name += to_wstring(atoi(argv[1]));
    wstr_e = new wchar_t[name.length()];
    wcscpy_s(wstr_e, name.length() + 1, name.c_str());
    WriteEnd = OpenEventW(EVENT_MODIFY_STATE, FALSE, wstr_e);
    if (WriteEnd == nullptr)
    {
        cout << "Open write end event failed." << endl;
        cout << "Input any char to exit." << endl;
        cin.get();
        return GetLastError();
    }

    //Запрашиваем ввод с консоли и запускаем соответствующие события
    string buffer;
    for (int i = 0, msg, nMsg = atoi(argv[0]); i < nMsg; i++)
    {
        WaitForSingleObject(hMutex, INFINITE);
        cout << "Event " << i + 1 << endl;
        cin >> buffer;
        if (buffer == "A") {
            SetEvent(AEvent);
        }
        else if (buffer == "B") {
            SetEvent(BEvent);
        }
        ReleaseMutex(hMutex);
    }
    //Отправляем событие завершения процесса
    SetEvent(WriteEnd);

    CloseHandle(hMutex);
    CloseHandle(AEvent);
    CloseHandle(BEvent);
    CloseHandle(WriteEnd);
    return 0;
}