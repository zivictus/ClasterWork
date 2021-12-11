#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <omp.h>
#include <unistd.h>
#include <vector>
#include <cstdlib>

using namespace std;

int getNumberElements(const char *path);

void  getMask(int numberToMask, int numberArray, int *powerOfTwo);

int* calculate(int inputNumber, int numberArray, int variants, int *array, int *powerOfTwo);


int main(int argc, char *argv[]) {

    int numberArray;
    char *arrayPath;
    int inputNumber;

    if (argc > 1)
    {
        arrayPath = argv[1];
        inputNumber = atoi(argv[2]);
        numberArray = getNumberElements(argv[1]);
    }
    else
    {
        inputNumber = 6;
        arrayPath = R"(D:\Magistratura\ClasterWork\1\Array.txt)";
        numberArray = getNumberElements(arrayPath);
    }

    auto *array = new int[numberArray];
    auto *powerOfTwo = new int[numberArray];

	int varriants =  pow(2, numberArray);

	for (int i = 0; i < numberArray; i++) {
        powerOfTwo[i] = pow(2,i);
    }


    ifstream file(arrayPath);
    if (!file.is_open()) {
        cout << "Error reading!" << endl;
    }
    else {
        string line;
        int count = 0;
        while (getline(file, line)) {
            int x;
            istringstream iss(line);
            iss >> x;
            array[count] = x;
            count++;
        }
    }
    file.close();

    clock_t begin = clock();
    int *pos = calculate(inputNumber, numberArray, varriants, array, powerOfTwo);
    clock_t end = clock();

    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    cout << "The sequential time: " << time_spent << " seconds\n";

    for (int i = 1; i < pos[0]+1; i++){
        getMask(pos[i], numberArray, powerOfTwo);
    }

    delete[] array;
    delete[] powerOfTwo;
    return 0;
}

int* calculate(int inputNumber, int numberArray, int variants, int *array, int *powerOfTwo) {

    int s = 0;
	int posCounter = 0;
    auto *possibilities = new int [variants];

#pragma omp parallel num_threads(variants)
    {
#pragma omp for
        for (int i = 0; i < variants; i++) {
            for (int j = 0; j < numberArray; j++) {
                if ((i & powerOfTwo[j]) != 0) {
                    s += array[j];
                }
            }

            if (inputNumber == s) {
                possibilities[posCounter + 1] = i;
                posCounter++;
            }

            s = 0;
        }
    };
    possibilities[0] = posCounter;
    return possibilities;
}

int getNumberElements(const char *path) {
    ifstream in(path);
    string str;
    int temp = 0;
    if (in.is_open()) {
        while (getline(in, str)) {
            temp++;
        }
    } else cout << "Error reading" << endl;
    in.close();
    return temp;
}

void getMask(int numberToMask, int numberArray, int *powerOfTwo)
{
    cout << "Mask: ";
    for(int j = 0; j < numberArray; j++)
    {
        if((numberToMask & powerOfTwo[j]) != 0)
        {
            cout << "1";
        } else
        {
            cout << "0";
        }
    }
    cout << "\n";
}
