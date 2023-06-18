#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

mutex critical_section;
mutex hOutEvent, hMultEvent;
condition_variable_any cv;

vector<double> finalArray;
double result = 1;
int k = 0;
double A;

void Work(vector<double>& arr, int size, int& k, double& A) {
    cout << "--- workThread is started" << endl;

    int finalArraytInterval;
    cout << "Enter time interval to finalArray after preparing each element: ";
    cin >> finalArraytInterval;

    finalArray = vector<double>(size);
    int finalArraySize = 0;
    for (int i = 0; i < size; ++i) {
        if (arr[i] < A) {
            finalArray[finalArraySize] = arr[i];
            finalArraySize++;
        }
        else {
            arr[i - finalArraySize] = arr[i];
        }

        this_thread::sleep_for(chrono::milliseconds(finalArraytInterval));
    }

    // Move the elements smaller than A to the end of the final array
    for (int i = 0; i < finalArraySize; i++) {
        arr[size - i - 1] = finalArray[i];
    }

    cout << "  finalArray elements: ";
    for (int i = 0; i < size; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
    cout << "--- Thread work is finished" << endl << endl;
    cv.notify_one();
}




void MultElement(vector<double>& arr, int size, int& k) {
    unique_lock<mutex> critical_section_locker(critical_section);
    unique_lock<mutex> event_locker(hMultEvent);
    cv.wait(event_locker);
    cout << "--- Thread MultElement is started" << endl;
    cout << "  ";

    for (int i = k; i < size; ++i) {
        result *= arr[i];
    }
    
    cout << "  Mult from position " << k << " to the end: " << result << endl;
    cout << "--- Thread MultElement is finished" << endl;
    //cv.notify_one();
    critical_section_locker.unlock();
}


int main() {
    // INPUT DATA
    int size;
    cout << "Enter size of array: ";
    cin >> size;
    cout << endl << "Enter elements of array: " << endl;

    vector<double> arr(size);

    for (int i = 0; i < size; ++i) {
        cin >> arr[i];
    }

    // PRINT INPUT DATA
    cout << endl << "-----------------------------------" << endl << endl;
    cout << "Input data: " << endl;
    cout << "Size: " << size << endl;
    cout << "Elements of array: ";
    for (int i = 0; i < size; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;

    // INITIALIZING
    cout << endl << "Enter k: ";
    cin >> k;
    cout << endl;


    cout << "Enter A: ";
    cin >> A;

    thread work(Work, ref(arr), size, ref(k), ref(A));
    thread multElement(MultElement, ref(arr), size, ref(k));

    this_thread::sleep_for(chrono::milliseconds(2));

    std::unique_lock<std::mutex> lock_out(hOutEvent);
    cv.wait(lock_out);

    cout << endl;

    cout << "Elements of array: ";
    for (int i = 0; i < size; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl << "Mult of Elements: " << result << endl;

    work.join();
    multElement.join();

    return 0;
}
