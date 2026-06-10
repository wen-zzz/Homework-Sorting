# 41343132
# 41343136

作業三

## 解題說明

本題要求實作五種核心排序演算法，並針對不同的資料量n=(500,1000,2000,3000,4000,5000)在最壞情況(Worst-case)與平均情況(Average-case)下的執行時間與記憶體用量進行精確的動態計時與實測分析。

要實作與評估的演算法包含：
1. Insertion Sort(插入排序)
2. Quick Sort(快速排序)
3. Merge Sort(合併排序)
4. Heap Sort(堆積排序)
5. Composite Sort(複合排序法)

---

## 程式實作

```cpp
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


struct MemoryResult {
    size_t currentKB;
    size_t peakKB;
};

MemoryResult getMemoryInfo() {
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return { pmc.WorkingSetSize / 1024, pmc.PeakWorkingSetSize / 1024 };
}

size_t mergeSortMemoryPeakKB = 0;


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


    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        mergeSortMemoryPeakKB = pmc.WorkingSetSize / 1024;
    }

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


void insertionSortWrapper(int arr[], int n) { insertionSort(arr, n); }
void quickSortWrapper(int arr[], int n) { quickSort(arr, 0, n - 1); }
void mergeSortWrapper(int arr[], int n) { mergeSortIterative(arr, n); }
void heapSortWrapper(int arr[], int n) { heapSort(arr, n); }
void compositeSortWrapper(int arr[], int n) { compositeSort(arr, 0, n - 1); }


vector<int> findWorstCasePermutation(int n, void (*sortFunc)(int[], int), int trials) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) arr[i] = i + 1;

    vector<int> worst_permutation = arr;
    long long max_duration_ns = 0;

    for (int t = 0; t < trials; t++) {
        Permute(arr.data(), n);
        vector<int> tmp = arr;

        auto start = chrono::high_resolution_clock::now();
        sortFunc(tmp.data(), n);
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        if (duration > max_duration_ns) {
            max_duration_ns = duration;
            worst_permutation = arr; 
        }
    }
    return worst_permutation;
}


int main() {
    //依需求修改N
    const int N =5000;
    const int NUM_PERMUTATIONS = 100;
    const int WORST_CASE_TRIALS = 10;

    srand((unsigned)time(NULL));

    cout << "======================================================" << endl;
    cout << " 排序演算法效能測試 (N=" << N << ", 重複次數=" << NUM_PERMUTATIONS << ")" << endl;
    cout << "======================================================" << endl << endl;


    vector<int> base_array(N);
    for (int i = 0; i < N; i++) base_array[i] = i + 1;

    vector<vector<int>> base_data(NUM_PERMUTATIONS, vector<int>(N));
    for (int i = 0; i < NUM_PERMUTATIONS; i++) {
        base_data[i] = base_array;
        Permute(base_data[i].data(), N);
    }

    auto run_test = [&](string name, vector<vector<int>>& data, void (*sortFunc)(int[], int), bool isMerge = false) {
        vector<vector<int>> test_data = data;
        mergeSortMemoryPeakKB = 0; 


        MemoryResult memStart = getMemoryInfo();

        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < NUM_PERMUTATIONS; i++) {
            sortFunc(test_data[i].data(), N);
        }
        auto end = chrono::high_resolution_clock::now();

        MemoryResult memEnd = getMemoryInfo();
        chrono::duration<double, std::milli> elapsed = end - start;

        cout << "   [" << name << "]" << endl;
        cout << "      時間: " << fixed << setprecision(2) << elapsed.count() << " ms" << endl;
        cout << "      執行前當前使用: " << memStart.currentKB << " KB" << endl;

        if (isMerge) {
     
            cout << "      執行期間最高峰值: " << mergeSortMemoryPeakKB << " KB" << endl;
            cout << "      (說明: 增加的量約為 N * 4 bytes = " << (N * 4 / 1024) << " KB)" << endl;
        }
        else {
      
            cout << "      執行後最高峰值:   " << memEnd.peakKB << " KB" << endl;
        }
        cout << "      執行後當前使用:   " << memEnd.currentKB << " KB" << endl << endl;
        };


    cout << ">>> Average-case <<<" << endl;
    run_test("Insertion Sort", base_data, insertionSortWrapper);
    run_test("Quick Sort", base_data, quickSortWrapper);
    run_test("Merge Sort", base_data, mergeSortWrapper, true);
    run_test("Heap Sort", base_data, heapSortWrapper);
    run_test("Composite Sort", base_data, compositeSortWrapper);


    cout << ">>> Worst-case <<<" << endl;


    vector<int> worst_ins_arr(N);
    for (int i = 0; i < N; i++) worst_ins_arr[i] = N - i;
    vector<vector<int>> worst_ins_data(NUM_PERMUTATIONS, worst_ins_arr);
    run_test("Insertion Sort", worst_ins_data, insertionSortWrapper);


    vector<int> worst_q_arr = findWorstCasePermutation(N, quickSortWrapper, WORST_CASE_TRIALS);
    vector<vector<int>> worst_q_data(NUM_PERMUTATIONS, worst_q_arr);
    run_test("Quick Sort", worst_q_data, quickSortWrapper);


    vector<int> worst_m_arr = findWorstCasePermutation(N, mergeSortWrapper, WORST_CASE_TRIALS);
    vector<vector<int>> worst_m_data(NUM_PERMUTATIONS, worst_m_arr);
    run_test("Merge Sort", worst_m_data, mergeSortWrapper, true);


    vector<int> worst_h_arr = findWorstCasePermutation(N, heapSortWrapper, WORST_CASE_TRIALS);
    vector<vector<int>> worst_h_data(NUM_PERMUTATIONS, worst_h_arr);
    run_test("Heap Sort", worst_h_data, heapSortWrapper);


    vector<int> worst_c_arr = findWorstCasePermutation(N, compositeSortWrapper, WORST_CASE_TRIALS);
    vector<vector<int>> worst_c_data(NUM_PERMUTATIONS, worst_c_arr);
    run_test("Composite Sort", worst_c_data, compositeSortWrapper);

    return 0;
}
```

## 效能分析
1. 時間、空間複雜度

|演算法|平均的時間|最壞的時間|空間複雜度|
|--|--|--|--|
|Insertion Sort|$O(n^2)$|$O(n^2)$|$O(1)$|
|Quick Sort|$O(n \log n)$|$O(n^2)$|$O(\log n)$|
|Merge Sort|$O(n \log n)$|$O(n \log n)$|$O(n)$ |
|Heap Sort|$O(n \log n)$|$O(n \log n)$|$O(1)$|
|Composite Sort|$O(n \log n)$|$O(n \log n)$ |$O(n)$ 或 $O(\log n)$|



## 測試與驗證
1.平均

|資料量 n|Insertion Sort (ms)|Quick Sort (ms)|Merge Sort (ms)|Heap Sort (ms)|Composite Sort (ms)|
|---|---|---|---|---|---|
|500|6.30|2.48|2.83|4.91|2.12|
|1000|25.58|5.46|6.41|11.88|4.60|
|2000|100.57|12.58| 13.42|25.44 |10.33|
|3000|236.66|21.78|21.64|44.97|23.16|
|4000|422.67|30.82|31.51|57.85|24.56|
|5000|639.53|34.36|37.98|70.48|29.18|


2.最壞

|資料量 n|Insertion Sort (ms)|Quick Sort (ms)|Merge Sort (ms)|Heap Sort (ms)|Composite Sort (ms)|
|---|---|---|---|---|---|
|500|11.48|0.87|1.47|3.94|0.61|
|1000|46.83|2.32|5.41|10.49|1.70 |
|2000|218.60|10.75|13.55|26.78 |8.21|
|3000|543.72|20.63|21.42|41.60|16.06|
|4000|813.20|23.33|29.12|55.01|21.58|
|5000|1123.78|30.03|34.43|66.18|25.82|

3.記憶體
>資料量500
>>Insertion Sort[Average|Worst]
>>>執行前當前使用: 4964 | 5728 KB

>>>執行後當前使用: 4964 | 5728 KB

>>>變化: 0 | 0 

>>Quick Sort[Average|Worst]
>>>執行前當前使用: 5068 | 5996 KB

>>>執行後當前使用: 5068 | 5996 KB

>>>變化: 0 | 0 
 
>>Merge Sort[Average|Worst]
>>>執行前當前使用: 5452 | 6016 KB

>>>執行後當前使用: 5452 | 6016 KB

>>>變化: 0 | 0 

>>Heap Sort[Average|Worst]
>>>執行前當前使用: 5452 | 6272 KB

>>>執行後當前使用: 5452 | 6272 KB

>>>變化: 0 | 0  

>>Composite Sort[Average|Worst]
>>>執行前當前使用: 5452 | 6556 KB

>>>執行後當前使用: 5452 | 6556 KB

>>>變化: 0 | 0

>資料量1000
>>Insertion Sort[Average|Worst]
>>>執行前當前使用: 5440 | 6584 KB

>>>執行後當前使用: 5440 | 6584 KB

>>>變化: 0 | 0 

>>Quick Sort[Average|Worst]
>>>執行前當前使用: 5796 | 6864 KB

>>>執行後當前使用: 5796 | 6864 KB

>>>變化: 0 | 0 
 
>>Merge Sort[Average|Worst]
>>>執行前當前使用: 6052 | 7392 KB

>>>執行後當前使用: 6052 | 7392 KB

>>>變化: 0 | 0 

>>Heap Sort[Average|Worst]
>>>執行前當前使用: 6052 | 7668 KB

>>>執行後當前使用: 6052 | 7668 KB

>>>變化: 0 | 0  

>>Composite Sort[Average|Worst]
>>>執行前當前使用: 6052 | 8192 KB

>>>執行後當前使用: 6052 | 8192 KB

>>>變化: 0 | 0

>資料量2000
>>Insertion Sort[Average|Worst]
>>>執行前當前使用: 6388 | 8308 KB

>>>執行後當前使用: 6388 | 8308 KB

>>>變化: 0 | 0 

>>Quick Sort[Average|Worst]
>>>執行前當前使用: 7000 | 9092 KB

>>>執行後當前使用: 7000 | 9092 KB

>>>變化: 0 | 0 
 
>>Merge Sort[Average|Worst]
>>>執行前當前使用: 7520 | 9876 KB

>>>執行後當前使用: 7520 | 10132 KB

>>>變化: 0 | 256 

>>Heap Sort[Average|Worst]
>>>執行前當前使用: 7520 | 10932 KB

>>>執行後當前使用: 7520 | 10932 KB

>>>變化: 0 | 0  

>>Composite Sort[Average|Worst]
>>>執行前當前使用: 7524 | 11712 KB

>>>執行後當前使用: 7524 | 11712 KB

>>>變化: 0 | 0

>資料量3000
>>Insertion Sort[Average|Worst]
>>>執行前當前使用: 7168 | 9864 KB

>>>執行後當前使用: 7168 | 9864 KB

>>>變化: 0 | 0 

>>Quick Sort[Average|Worst]
>>>執行前當前使用: 7780 | 11168 KB

>>>執行後當前使用: 7780 | 11168 KB

>>>變化: 0 | 0 
 
>>Merge Sort[Average|Worst]
>>>執行前當前使用: 8556 | 12464 KB

>>>執行後當前使用: 8556 | 12464 KB

>>>變化: 0 | 0 

>>Heap Sort[Average|Worst]
>>>執行前當前使用: 8556 | 13512 KB

>>>執行後當前使用: 8556 | 13512 KB

>>>變化: 0 | 0  

>>Composite Sort[Average|Worst]
>>>執行前當前使用: 8556 | 14812 KB

>>>執行後當前使用: 8556 | 14812 KB

>>>變化: 0 | 0

>資料量4000
>>Insertion Sort[Average|Worst]
>>>執行前當前使用: 8000 | 9888 KB

>>>執行後當前使用: 8000 | 9880 KB

>>>變化: 0 | -8 

>>Quick Sort[Average|Worst]
>>>執行前當前使用: 8100 | 11712 KB

>>>執行後當前使用: 8100 | 11712 KB

>>>變化: 0 | 0 
 
>>Merge Sort[Average|Worst]
>>>執行前當前使用: 8100 | 13400 KB

>>>執行後當前使用: 8100 | 13560 KB

>>>變化: 0 | 160 

>>Heap Sort[Average|Worst]
>>>執行前當前使用: 8100 | 15128 KB

>>>執行後當前使用: 8100 | 15128 KB

>>>變化: 0 | 0  

>>Composite Sort[Average|Worst]
>>>執行前當前使用: 8100 | 16884 KB

>>>執行後當前使用: 8100 | 16884 KB

>>>變化: 0 | 0

>資料量5000
>>Insertion Sort[Average|Worst]
>>>執行前當前使用: 8564 | 10720 KB

>>>執行後當前使用: 8564 | 10720 KB

>>>變化: 0 | 0 

>>Quick Sort[Average|Worst]
>>>執行前當前使用: 8664 | 12768 KB

>>>執行後當前使用: 8664 | 12768 KB

>>>變化: 0 | 0 
 
>>Merge Sort[Average|Worst]
>>>執行前當前使用: 8664 | 14824 KB

>>>執行後當前使用: 8684 | 14844 KB

>>>變化: 20 | 20 

>>Heap Sort[Average|Worst]
>>>執行前當前使用: 8664 | 16876 KB

>>>執行後當前使用: 8664 | 16876 KB

>>>變化: 0 | 0  

>>Composite Sort[Average|Worst]
>>>執行前當前使用: 8664 | 18924 KB

>>>執行後當前使用: 8664 | 18924 KB

>>>變化: 0 | 0





## 申論及開發報告

時間：當資料量 $n$ 從 500 成長至 5000 時，可以看到 Insertion Sort 的時間呈現平方的增加。相較之下，Quick, Merge, Heap Sort 則呈現穩定的成長。Composite Sort 在隨機資料時，其消耗時間比其他排序法少。證明在資料極小時，利用Iertion Sort 代替，能有效減少運算常數。空間記憶體測量中，只有 Merge Sort 在配置 temp 陣列的期間出現了KB級增量，且該增量隨著資料量n的增加呈線性成長，這符合空間複雜度為 $O(n)$ 的理論。而其他排序法在執行期間的記憶體增量皆保持為 0，驗證了 $O(1)$ 的空間複雜度。
