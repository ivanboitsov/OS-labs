#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <windows.h>
#include <string>

using namespace std;

mutex mtx;
condition_variable cv;
int activeThreads = 0;
int maxThreads;

//Четно-нечетная сортировка Бетчера
void oddEvenSort(vector<int>& arr, int start, int step) {
	for (int i = start; i + 1 < arr.size(); i += step) {
		if (arr[i] > arr[i + 1]) {
			swap(arr[i], arr[i + 1]);
		}
	}
}

void threadWorker(vector<int>& arr, int step, int index) {
	{
		unique_lock<mutex> lock(mtx);
		cv.wait(lock, [] { return activeThreads < maxThreads; });
		activeThreads++;
	}

	oddEvenSort(arr, index, 2);

	{
		lock_guard<mutex> lock(mtx);
		activeThreads--;
		cv.notify_one();
	}
}
//Параллельная четно-нечетная сортировка Бетчера
void oddEvenSortParallel(vector<int>& arr, int maxThreads) {
	bool sorted = false;
	while (!sorted) {
		sorted = true;

		vector<thread> threads;

		//Четная фаза
		for (int i = 0; i < 2; i++) {
			threads.emplace_back(threadWorker, ref(arr), 2, i);
			Sleep(2000);
		}

		for (auto& t : threads) {
			Sleep(2000);
			t.join();
		}

		//Проверка на отсортированность массива
		for (int i = 0; i + 1 < arr.size(); i++) {
			if (arr[i] > arr[i + 1]) {
				sorted = false;
				break;
			}
		}
	}
}
int main(int args, char* argv[]) {

	if (args < 2) {
		cerr << "Usage: " << argv[0] << " <maxThreads>" << endl;
		system("pause");
		return 1;
	}

	maxThreads = stoi(argv[1]);

	vector<int> arr = { 24, 11, 13, 4, 6, 22, 15, 9, 1, 10 };
	cout << "Original array: ";

	for (int num : arr) {
		cout << num << " ";
	}

	cout << endl;

	oddEvenSortParallel(arr, maxThreads);

	cout << "Sorted array: ";
	for (int num : arr) {
		cout << num << " ";
	}
	cout << endl;

	system("pause");
	return 0;
}
