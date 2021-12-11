#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <mpi.h>

#define send_data_tag 2001
#define return_data_tag 2002

using namespace std;

int getNumberElements(const char *path);

void getMask(int numberToMask, int numberArray, int *powerOfTwo);

int *calculate(int start_row, int end_row, int inputNumber, int numberArray, int variants, int *array, int *powerOfTwo);

int *Concatinate(int *first, int *second);


int main(int argc, char *argv[]) {

    int numberArray;
    char *arrayPath;
    int inputNumber;
    MPI_Comm Comm = MPI_COMM_WORLD;
    MPI_Status status;
    double time;

    int my_id, root_process, ierr, i, num_procs,
            an_id, num_rows_to_receive, avg_rows_per_process,
            sender, num_rows_received, start_row, end_row, num_rows_to_send,
            start_row_to_receive, end_row_to_receive;

    root_process = 0;

    if (argc > 1) {
        arrayPath = argv[1];
        inputNumber = atoi(argv[2]);
        numberArray = getNumberElements(argv[1]);
    } else {
        inputNumber = 6;
        arrayPath = R"(D:\Magistratura\ClasterWork\1\Array.txt)";
        numberArray = getNumberElements(arrayPath);
    }

    int variants = pow(2, numberArray);

    auto *array = new int[numberArray];
    auto *array2 = new int[numberArray];

    auto *powerOfTwo = new int[numberArray];

    auto *possibilities = new int[variants];
    auto *possibilities2 = new int[variants];
    auto *partial_possibilities = new int[variants];

    ierr = MPI_Init(&argc, &argv);

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (my_id == root_process) {

        for (int i = 0; i < numberArray; i++) {
            powerOfTwo[i] = pow(2, i);
        }


        ifstream file(arrayPath);
        if (!file.is_open()) {
            cout << "Error reading!" << endl;
        } else {
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


        avg_rows_per_process = variants / num_procs;

        for(an_id = 1; an_id < num_procs; an_id++) {
            start_row = an_id*avg_rows_per_process+1;
            end_row   = (an_id + 1)*avg_rows_per_process;

            ierr = MPI_Send( &start_row, 1 , MPI_INT,
                             an_id, send_data_tag, MPI_COMM_WORLD);

            ierr = MPI_Send( &end_row, 1, MPI_INT,
                             an_id, send_data_tag, MPI_COMM_WORLD);
        }

        possibilities = calculate(
                1,
                avg_rows_per_process,
                inputNumber,
                numberArray,
                avg_rows_per_process ,
                array,
                powerOfTwo);

        for(an_id = 1; an_id < num_procs; an_id++) {

            ierr = MPI_Recv( &possibilities2, 1, MPI_LONG, MPI_ANY_SOURCE,
                             return_data_tag, MPI_COMM_WORLD, &status);

            possibilities = Concatinate(possibilities, possibilities2);
        }

        for (int i = 1; i < (sizeof(possibilities)/sizeof(possibilities[0])); i++) {
            getMask(possibilities[i], numberArray, powerOfTwo);
        }

        delete[] array;
        delete[] powerOfTwo;

        return 0;

    } else
    {
        ierr = MPI_Recv( &start_row_to_receive, 1, MPI_INT,
                         root_process, send_data_tag, MPI_COMM_WORLD, &status);

        ierr = MPI_Recv( &end_row_to_receive, num_rows_to_receive, MPI_INT,
                         root_process, send_data_tag, MPI_COMM_WORLD, &status);

        num_rows_received = num_rows_to_receive;

        partial_possibilities = calculate(
                start_row_to_receive,
                end_row_to_receive,
                inputNumber,
                numberArray,
                avg_rows_per_process ,
                array, powerOfTwo);

        ierr = MPI_Send( &partial_possibilities, 1, MPI_LONG, root_process,
                         return_data_tag, MPI_COMM_WORLD);
    }
        ierr = MPI_Finalize();
}

int *calculate(int start_row, int end_row ,int inputNumber, int numberArray, int variants, int *array, int *powerOfTwo) {

    int s = 0;
    int posCounter = 0;
    auto *possibilities = new int[variants];

    for (int i = start_row; i < end_row+1; i++) {
        for (int j = 0; j < numberArray; j++) {
            if ((i & powerOfTwo[j]) != 0) {
                s += array[j];
            }
        }

        if (inputNumber == s) {
            possibilities[posCounter] = i;
        }

        s = 0;
    }
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

void getMask(int numberToMask, int numberArray, int *powerOfTwo) {
    cout << "Mask: ";
    for (int j = 0; j < numberArray; j++) {
        if ((numberToMask & powerOfTwo[j]) != 0) {
            cout << "1";
        } else {
            cout << "0";
        }
    }
    cout << "\n";
}

int *Concatinate(int *first, int *second)
{

    int m = sizeof(first)/sizeof(first[0]);
    int n = sizeof(second)/sizeof(second[0]);

    int arr[m + n];
    copy(first, first + m, arr);
    copy(second, second + n, arr + m);

    return arr;
}
