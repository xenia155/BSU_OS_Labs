#include <iostream>
#include <windows.h>
#include <conio.h>
using namespace std;

void swap(int arr[], int pos1, int pos2) {
	int temp;
	temp = arr[pos1];
	arr[pos1] = arr[pos2];
	arr[pos2] = temp;
}

int partition(int arr[], int low, int high, int pivot) {
	int i = low;
	int j = low;
	while (i <= high) {
		if (arr[i] > pivot) {
			i++;
		}
		else {
			swap(arr, i, j);
			i++;
			j++;
		}
	}
	return j - 1;
}

void quickSort(int arr[], int low, int high) {
	if (low < high) {
		int pivot = arr[high];
		int pos = partition(arr, low, high, pivot);

		quickSort(arr, low, pos - 1);
		quickSort(arr, pos + 1, high);
	}
}

int main(int args, char* argv[])
{
	//int color = atoi(argv[0]);
	//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleTextAttribute(hConsole, color);//устанавливает цвет текста

	cout << "Mas: ";//вывод массива
	int* mas = new int[args - 1];
	for (int i = 0; i < args - 1; i++) {
		mas[i] = atoi(argv[i]);
		cout << mas[i] << " ";
	}
	cout << endl;

	/*int j = 0;//ищет числа, кратные 5, и переставляет их
	for (int i = 0; i < args - 1; i++) {
		if (mas[i] % 5 == 0) {
			int byf = mas[i];
			mas[i] = mas[j];
			mas[j] = byf;
			j++;
		}
	}*/
	quickSort(mas, 0, args - 2);

	cout << "New formed mas: ";//вывод массива
	for (int i = 0; i < args - 1; i++) {
		cout << mas[i] << " ";
	}

	int a;
	cin >> a;
}