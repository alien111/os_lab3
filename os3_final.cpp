#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <pthread.h>
#include <memory>

int numberOfThreads;
std::vector<std::string> vec;

class threadData_t {
public:

	int lhs;
	int rhs;

	threadData_t() = default;
	threadData_t(int l, int r) {	
		lhs = l;
		rhs = r;
	}
};


int compare(std::string& a, std::string& b) {
	if (a.size() < b.size()) {
        return -1;                                // left is less -1
    } else if (b.size() < a.size()) {
        return 1;                                 // right is less 1
    }

    for (int i = 0; i < b.size(); i++) {
        if (a[i] < b[i]) {
            return -1;
        } else if (b[i] < a[i]) {
            return 1;
        }
    }

    return 0;
}

void compareExchange(std::string& a, std::string& b) {

	if (b.compare(a) < 0)
		std::swap(a, b);

	/*

	if (a.size() < b.size()) {
        return;
    } else if (b.size() < a.size()) {
        std::swap(a, b);
        return;
    }

    for (int i = 0; i < b.size(); i++) {
        if (a[i] < b[i]) {
            return;
        } else if (b[i] < a[i]) {
            std::swap(a, b);
            return;
        }
    }*/
}



void * nonRecursiveBatcherSort(void * arg) {

	threadData_t * data = (threadData_t *) arg;

	int l = data -> lhs;
	int r = data -> rhs;
	
	int N = r - l + 1;
	for (int p = 1; p < N; p += p) 
		for (int k = p; k > 0; k /= 2)
			for (int j = k % p; j + k < N; j += (k + k))
				for (int i = 0; i < N - j - k; i++) 
					if ((j + i) / (p + p) == (j + i + k) / (p + p)){
						compareExchange(vec[l + j + i], vec[l + j + i + k]);


					}



}

void merge(void * arg) {
	threadData_t * data = (threadData_t *) arg;

	int l = data -> lhs;
	int r = data -> rhs;

	int mid = (l + r) / 2;

	int size = r - l + 1;

	std::vector<std::string> buf(size);

	int first = l;
	int second = mid + 1;

	for (int i = 0; i < size; i++) {
		if (first > mid) {
			buf[i] = vec[second++];
			continue;
		}
		if (second > r) {
			buf[i] = vec[first++];
			continue;
		}
		//if (compare(vec[first], vec[second]) < 0) 
			//buf[i] = vec[first++];
		if (vec[second].compare(vec[first]) < 0)
			buf[i] = vec[first++];
		else 
			buf[i] = vec[second++];
	}

	for (int i = l, k = 0; i <= r; i++, k++) {
		vec[i] = buf[k];
	}
}

void mergeParts(void * arg) {

	threadData_t * data = (threadData_t *) arg;

	int l = data -> lhs;
	int r = data -> rhs;

	if (r - l > 0) {
		int mid = (l + r) / 2;
		threadData_t left(l, mid);
		threadData_t right(mid + 1, r);
		mergeParts(&left);
		mergeParts(&right);

		threadData_t merge0(l, r);
		merge(&merge0);
			
	}
}

int main(int argc, char * argv[]) {

	if (argc < 2) {
		numberOfThreads = 4;
		std::cout << "NO ARGV GIVEN\n";
	} else {
		numberOfThreads = atoi(argv[1]);
	}


	std::ifstream input("input.txt");
	std::ofstream output("output.txt");

	int size;

	input >> size;



	for (int i = 0; i < size; i++) {
		std::string buf;
		input >> buf;
		vec.push_back(buf);
	}



	pthread_t threads[numberOfThreads];

	int step = size / numberOfThreads;
	int l = 0;
	int r = step;

	for (int i = 0; i < numberOfThreads - 1; i++) {
		threadData_t data(l + i * step, r + i * step);

		int res = pthread_create(&threads[i], NULL, &nonRecursiveBatcherSort, &data);

		if (res) {
			std::cout << "Errors occured!\n";
		}
		
		pthread_join(threads[i], NULL);	
	}

	threadData_t data(l + (numberOfThreads - 1) * step, size - 1);


	int res = pthread_create(&threads[numberOfThreads - 1], NULL, &nonRecursiveBatcherSort, &data);

	if (res) {
		std::cout << "Errors occured!\n";
	}
		
	pthread_join(threads[numberOfThreads - 1], NULL);	

	threadData_t dataF(0, size - 1);
	mergeParts(&dataF);

	for (int i = 0; i < size; i++) {
		output << vec[i] << '\n';
	}



	input.close();
	output.close();


	std::cout << "Program used " << numberOfThreads << " threads!\n";


	return 0;
}