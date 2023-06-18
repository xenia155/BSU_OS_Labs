#include <windows.h>
#include <iostream>
#include <string>
#pragma comment(lib, "kernel32.lib")

using namespace std;

int main(int argc, char* argv[])
{
    HANDLE hSemaphore;
    HANDLE CEvent = new HANDLE;
    HANDLE DEvent = new HANDLE;
    HANDLE ReadEnd;
    cout << "Reader " << atoi(argv[1]) << endl;
    //�������� �������
    hSemaphore = OpenSemaphoreW(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, L"Semaphore");
    if (hSemaphore == NULL)
    {
        DWORD dwLastError = GetLastError();
        cout << "OpenSemaphore failed with error " << dwLastError << endl;
        cout << "Press any key to exit." << endl;
        cin.get();
        return dwLastError;
    }

    //�������� ������� � D � �������� �������
    std::wstring name = L"CEvent_";
    name += to_wstring(atoi(argv[1]));
    auto* wstr_e = new wchar_t[name.length()];
    wcscpy_s(wstr_e, name.length() + 1, name.c_str());

    CEvent = OpenEventW(EVENT_ALL_ACCESS, FALSE, wstr_e);
    if (CEvent == nullptr)
    {
        cout << "Open event failed." << endl;
        cout << "Input any char to exit." << endl;
        cin.get();
        return GetLastError();
    }

    name = L"DEvent_";
    name += to_wstring(atoi(argv[1]));
    auto* wstr_ee = new wchar_t[name.length()];
    wcscpy_s(wstr_ee, name.length() + 1, name.c_str());

    DEvent = OpenEventW(EVENT_ALL_ACCESS, FALSE, wstr_ee);
    if (DEvent == nullptr)
    {
        cout << "Open ending event failed." << endl;
        cout << "Input any char to exit." << endl;
        cin.get();
        return GetLastError();
    }

    name = L"ReadEndEvent_";
    name += to_wstring(atoi(argv[1]));
    wstr_e = new wchar_t[name.length()];
    wcscpy_s(wstr_e, name.length() + 1, name.c_str());
    ReadEnd = OpenEventW(EVENT_ALL_ACCESS, FALSE, wstr_e);
    if (ReadEnd == nullptr)
    {
        cout << "Open write end event failed." << endl;
        cout << "Input any char to exit." << endl;
        cin.get();
        return GetLastError();
    }

    //���� ������� �� �������� Administrator � ������� �������������� ������(C ��� D)
    for (int i = 0, nMsg = atoi(argv[0]); i < nMsg; i++)
    {
        WaitForSingleObject(hSemaphore, INFINITE);
        cout << "Active reader\n";
        int num = WaitForMultipleObjects(2, new HANDLE[]{CEvent, DEvent}, FALSE, 0) - WAIT_OBJECT_0;
        switch (num) {
        case 0: // ���� ��������� ������� C
            cout << "Reader " << atoi(argv[1]) << ": C\n"; // ������� �������� C
            break;
        case 1: // ���� ��������� ������� D
            cout << "Reader " << atoi(argv[1]) << ": D\n"; // ������� �������� D
            break;
        default:
            cout << "Wrong code " << GetLastError() <<"\n";
            break;
        }
        ReleaseSemaphore(hSemaphore, 1, NULL); // ����������� ����� � ��������
    }
    SetEvent(ReadEnd);

    CloseHandle(hSemaphore);
    CloseHandle(CEvent); 
    CloseHandle(DEvent);
    CloseHandle(ReadEnd);
    system("pause");
    return 0;
}