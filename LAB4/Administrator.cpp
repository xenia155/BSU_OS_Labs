#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#pragma comment(lib, "kernel32.lib")

using namespace std;
int main()
{
    //Создаем мьютекс
    HANDLE hMutex = NULL;
    hMutex = CreateMutexW(NULL, FALSE, L"Mutex");
    if (hMutex == NULL)
    {
        cout << "Create mutex failed." << endl;
        cout << "Press any key to exit." << endl;
        cin.get();
        return GetLastError();
    }
    //Создаем семафор
    HANDLE hSemaphore = NULL;
    hSemaphore = CreateSemaphoreW(NULL, 2, 2, L"Semaphore");
    if (hSemaphore == NULL)
    {
        cout << "Create semaphore failed." << endl;
        cout << "Press any key to exit." << endl;
        cin.get();
        return GetLastError();
    }

    //Инициализируем счетчики и считываем количество процессов
    int nWriterProcs;
    int nReaderProcs;
    int nWriteMsg;
    int nReadMsg;
    cout << "Enter the number of Writer processes: ";
    cin >> nWriterProcs;
    cout << "Enter the number of Reader processes: ";
    cin >> nReaderProcs;

    //Инициализация событий и начальной информации процессов
    HANDLE* AEvent = new HANDLE[nWriterProcs];
    HANDLE* BEvent = new HANDLE[nWriterProcs];
    HANDLE* CEvent = new HANDLE[nReaderProcs];
    HANDLE* DEvent = new HANDLE[nReaderProcs];
    HANDLE* ReadEnd = new HANDLE[nReaderProcs];
    HANDLE* WriteEnd = new HANDLE[nWriterProcs];

    vector<STARTUPINFOW*> siWriter(nWriterProcs);
    vector<STARTUPINFOW*> siReader(nReaderProcs);
    vector<PROCESS_INFORMATION*> piWriter(nWriterProcs);
    vector<PROCESS_INFORMATION*> piReader(nReaderProcs);
    wstring temp;
    int writeMsg = 0, readMsg = 0;
    wchar_t* lpszCommandLine;
    //Создание процессов Writer
    for (int i = 0; i < nWriterProcs; i++)
    {
        //Создаем событие А
        std::wstring name = L"AEvent_";
        name += to_wstring(i + 1);
        auto* wstr_e = new wchar_t[name.length()];
        wcscpy_s(wstr_e, name.length() + 1, name.c_str());

        AEvent[i] = CreateEventW(NULL, FALSE, FALSE, wstr_e);
        if (AEvent[i] == NULL)
        {
            return GetLastError();
        }

        //Создаем событие В
        name = L"BEvent_";
        name += to_wstring(i + 1);
        wstr_e = new wchar_t[name.length()];
        wcscpy_s(wstr_e, name.length() + 1, name.c_str());

        BEvent[i] = CreateEventW(NULL, FALSE, FALSE, wstr_e);
        if (BEvent[i] == NULL)
        {
            return GetLastError();
        }

        //Создаем событие завершения процесса Writer
        name = L"WriteEndEvent_";
        name += to_wstring(i + 1);
        wstr_e = new wchar_t[name.length()];
        wcscpy_s(wstr_e, name.length() + 1, name.c_str());

        WriteEnd[i] = CreateEventW(NULL, FALSE, FALSE, wstr_e);
        if (WriteEnd[i] == NULL)
        {
            return GetLastError();
        }

        //задаем начальную информацию для процесса
        siWriter[i] = new STARTUPINFOW;
        ZeroMemory(siWriter[i], sizeof(STARTUPINFOW));
        siWriter[i]->cb = sizeof(STARTUPINFOW);
        piWriter[i] = new PROCESS_INFORMATION;

        //Считываем количество сообщений для Writer
        cout << "Enter the number of messages from Writer " << i + 1 << ": ";
        cin >> nWriteMsg;
        writeMsg += nWriteMsg;

        //Задаем аргументы для процесса Writer
        temp = to_wstring(nWriteMsg) + L" " + to_wstring(i + 1);
        lpszCommandLine = new wchar_t[temp.length()];
        wcscpy_s(lpszCommandLine, temp.length() + 1, temp.c_str());

        //Создаем процесс и проверяем
        if (!CreateProcessW(L"Writer.exe", lpszCommandLine, NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, nullptr, NULL, siWriter[i], piWriter[i]))
        {
            cout << "The new process is not created." << endl;
            cout << "Press any key to exit." << endl;
            cin.get();
            CloseHandle(hMutex);
            return GetLastError();
        }
    }

    //Создание процессов Reader
    for (int i = 0; i < nReaderProcs; i++)
    {
        //Создаем событие С
        std::wstring name = L"CEvent_";
        name += to_wstring(i + 1);
        auto* wstr_e = new wchar_t[name.length()];
        wcscpy_s(wstr_e, name.length() + 1, name.c_str());

        CEvent[i] = CreateEventW(NULL, TRUE, FALSE, wstr_e);
        if (CEvent[i] == NULL)
        {
            return GetLastError();
        }

        //Создаем событие D
        name = L"DEvent_";
        name += to_wstring(i + 1);
        auto* wstr_ee = new wchar_t[name.length()];
        wcscpy_s(wstr_ee, name.length() + 1, name.c_str());

        DEvent[i] = CreateEventW(NULL, TRUE, FALSE, wstr_ee);
        if (DEvent[i] == NULL)
        {
            return GetLastError();
        }

        //Создаем событие завершения процесса Reader
        name = L"ReadEndEvent_";
        name += to_wstring(i + 1);
        wstr_e = new wchar_t[name.length()];
        wcscpy_s(wstr_e, name.length() + 1, name.c_str());

        ReadEnd[i] = CreateEventW(NULL, FALSE, FALSE, wstr_e);
        if (ReadEnd[i] == NULL)
        {
            return GetLastError();
        }

        //Задаем стартовые параметры процесса
        siReader[i] = new STARTUPINFOW;
        ZeroMemory(siReader[i], sizeof(STARTUPINFOW));
        siReader[i]->cb = sizeof(STARTUPINFOW);
        piReader[i] = new PROCESS_INFORMATION;

        //Считываем количество сообщений
        cout << "Enter the number of messages for Reader " << i + 1 << ": ";
        cin >> nReadMsg;
        readMsg += nReadMsg;
        //Аргументы для процесса
        temp = to_wstring(nReadMsg) + L" " + to_wstring(i + 1);
        lpszCommandLine = new wchar_t[temp.length()];
        wcscpy_s(lpszCommandLine, temp.length() + 1, temp.c_str());

        //Передаем событие C или D(можно и после создания процесса)
        std::cout << "Print C or D for Reader " << i + 1 << '\n';
        char ch;
        std::cin >> ch;
        switch (ch) {
        case 'C':
            SetEvent(CEvent[i]);
            break;
        case 'D':
            SetEvent(DEvent[i]);
            break;
        }

        //Создаем процесс
        if (!CreateProcessW(L"Reader.exe", lpszCommandLine, NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, nullptr, NULL, siReader[i], piReader[i]))
        {
            cout << "The new process is not created." << endl;
            cout << "Press any key to exit." << endl;
            cin.get();
            return GetLastError();
        }
    }

    for (int i = 0; i < nWriterProcs; i++) {
        //Ждем выполнения события от процесса Writer
        int prEventIndex = WaitForMultipleObjects(3, new HANDLE[]{AEvent[i],BEvent[i],WriteEnd[i]}, FALSE, INFINITE) - WAIT_OBJECT_0;
        cout << "Writer process " << i + 1 << " returns ";
        //Выводим результат работы процесса Writer
        switch (prEventIndex) {
        case 0:
            cout << "A";
            break;
        case 1:
            cout << "B";
            break;
        case 2:
            cout << "Not A not B but ended";
            break;
        } 
        cout << endl;
    }

    //Ждем выполнения процесса Reader
    for (int i = 0; i < nReaderProcs; i++) {
        DWORD dwWaitResult = WaitForSingleObject(ReadEnd[i], INFINITE);
        cout << "Reader " << i + 1;
        if (dwWaitResult == WAIT_OBJECT_0) {
            cout << " finished.\n";
        }
        else {
            cout << " error.\n";
        }
    }

    //Закрываем все события, мьютекс, семафор и хендлы
    for (int i = 0; i < nWriterProcs; i++)
    {
        WaitForSingleObject(piWriter[i]->hProcess, INFINITE);
        CloseHandle(piWriter[i]->hThread);
        CloseHandle(piWriter[i]->hProcess);
        CloseHandle(AEvent[i]);
        CloseHandle(BEvent[i]);
        delete siWriter[i];
        delete piWriter[i];
    }

    for (int i = 0; i < nReaderProcs; i++)
    {
        WaitForSingleObject(piReader[i]->hProcess, INFINITE);
        CloseHandle(piReader[i]->hThread);
        CloseHandle(piReader[i]->hProcess);
        CloseHandle(CEvent[i]);
        CloseHandle(DEvent[i]);
        delete siReader[i];
        delete piReader[i];
    }

    CloseHandle(hMutex);
    CloseHandle(hSemaphore);
    return 0;
}