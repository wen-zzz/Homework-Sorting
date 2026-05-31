#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
using namespace std;

void printMemoryUsage(const string& processName) {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        cout << left << setw(20) << processName
            << " | 目前使用: " << right << setw(6) << pmc.WorkingSetSize / 1024 << " KB"
            << " | 最高峰值(Peak): " << right << setw(6) << pmc.PeakWorkingSetSize / 1024 << " KB\n";
    }
}

template <class T>
void Permute(T* a, int n) {
    for (int i = n - 1; i >= 1; i--) {
        int j = rand() % (i + 1);
        swap(a[j], a[i]);
    }
}

void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void insertionSortRange(int arr[], int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int partition(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    if (arr[mid] < arr[low]) swap(arr[mid], arr[low]);
    if (arr[high] < arr[low]) swap(arr[high], arr[low]);
    if (arr[high] < arr[mid]) swap(arr[high], arr[mid]);

    swap(arr[mid], arr[high - 1]);
    int pivot = arr[high - 1];
    int i = low;
    int j = high - 1;

    while (true) {
        while (arr[++i] < pivot) {}
        while (arr[--j] > pivot) {}
        if (i >= j) break;
        swap(arr[i], arr[j]);
    }
    swap(arr[i], arr[high - 1]);
    return i;
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        if (high - low + 1 > 3) {
            int pi = partition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
        else {
            insertionSort(arr + low, high - low + 1);
        }
    }
}

void merge(int arr[], int temp[], int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (i = left; i <= right; i++) arr[i] = temp[i];
}

void mergeSortIterative(int arr[], int n) {
    int* temp = new int[n];
    for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
            int mid = min(left_start + curr_size - 1, n - 1);
            int right_end = min(left_start + 2 * curr_size - 1, n - 1);
            merge(arr, temp, left_start, mid, right_end);
        }
    }
    delete[] temp; 
}

void heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(int arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

void compositeSort(int arr[], int low, int high) {
    int threshold = 15;
    if (high - low + 1 <= threshold) {
        insertionSortRange(arr, low, high);
    }
    else {
        int pi = partition(arr, low, high);
        compositeSort(arr, low, pi - 1);
        compositeSort(arr, pi + 1, high);
    }
}

int main() { 
    const int N = 5000;             
    const int NUM_PERMUTATIONS = 100; 

    srand((unsigned)time(NULL));

    cout << "======================================================\n";
    cout << " 演算法效能與記憶體測試 (N = " << N << ", 排列數 = " << NUM_PERMUTATIONS << ")\n";
    cout << "======================================================\n\n";

    printMemoryUsage("[初始狀態]");
    vector<int> base_array(N);
    for (int i = 0; i < N; i++) base_array[i] = i + 1;

    vector<vector<int>> base_data(NUM_PERMUTATIONS, vector<int>(N));
    for (int i = 0; i < NUM_PERMUTATIONS; i++) {
        base_data[i] = base_array;
        Permute(base_data[i].data(), N);
    }

    printMemoryUsage("[產生亂數資料後]");
    cout << "------------------------------------------------------\n";

    vector<vector<int>> test_data;
    chrono::time_point<chrono::high_resolution_clock> start, end;
    chrono::duration<double, std::milli> elapsed;
    test_data = base_data;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_PERMUTATIONS; i++) {
        insertionSort(test_data[i].data(), N);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "1. Insertion Sort \t耗時: " << fixed << setprecision(2) << elapsed.count() << " ms\n";
    printMemoryUsage("   -> 記憶體狀態");
    test_data = base_data;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_PERMUTATIONS; i++) {
        quickSort(test_data[i].data(), 0, N - 1);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "2. Quick Sort \t\t耗時: " << fixed << setprecision(2) << elapsed.count() << " ms\n";
    printMemoryUsage("   -> 記憶體狀態");
    test_data = base_data;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_PERMUTATIONS; i++) {
        mergeSortIterative(test_data[i].data(), N);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "3. Merge Sort \t\t耗時: " << fixed << setprecision(2) << elapsed.count() << " ms\n";
    printMemoryUsage("   -> 記憶體狀態");
    test_data = base_data;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_PERMUTATIONS; i++) {
        heapSort(test_data[i].data(), N);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "4. Heap Sort \t\t耗時: " << fixed << setprecision(2) << elapsed.count() << " ms\n";
    printMemoryUsage("   -> 記憶體狀態");
    test_data = base_data;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_PERMUTATIONS; i++) {
        compositeSort(test_data[i].data(), 0, N - 1);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "5. Composite Sort \t耗時: " << fixed << setprecision(2) << elapsed.count() << " ms\n";
    printMemoryUsage("   -> 記憶體狀態");

    cout << "\n測試完成！\n";
    return 0;
}