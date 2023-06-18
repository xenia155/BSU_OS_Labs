#include <iostream>
#include <windows.h>
#include <conio.h>
using namespace std;

int main()
{
	STARTUPINFO si;

	// Структура piApp типа PROCESS_INFORMATION содержит идентификаторы и дескрипторы нового создаваемого процесса и его главного потока. 
	PROCESS_INFORMATION piApp; 

	//устанавливаем атрибуты нового процесса
	ZeroMemory(&si, sizeof(STARTUPINFO)); // все поля структуры si типа STARTUPINFO заполняются нулями.
	si.cb = sizeof(STARTUPINFO);

	si.dwFlags = STARTF_USEFILLATTRIBUTE; // этот элемент является исходным текстом и цветом фона, если в консольном приложении создается новое окно консоли. В противном случае этот элемент игнорируется.
	si.dwFillAttribute = BACKGROUND_GREEN;
	//si.dwFillAttribute = FOREGROUND_GREEN;

	wchar_t args[] = L"12 50 67 23 45 10 78 17";

	// запускаем новый консольный процесс
	CreateProcess(L"processChild.exe", args, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp);
	_cputs("The new process is created.\n");

	// ждем закрытия процесса
	WaitForSingleObject(piApp.hProcess, INFINITE);

	// Мы не используем эти дескрипторы в нашей программе и поэтому закрываем их
	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);
}
