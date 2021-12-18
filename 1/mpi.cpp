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

///Получить из файла количество элементов массива
int getNumberElements(const char* path);

///Вывести маску массива
void getMask(int numberToMask, int numberArray, int* powerOfTwo);

///Вычислить маску массива
void calculate(int start_row, int end_row, int inputNumber, int numberArray, int variants, int* array, int* powerOfTwo);


int main(int argc, char* argv[]) {

	int numberArray; //Размер массива
	const char* arrayPath; //Путь к массиву
	int inputNumber; //Число для поиска суммы
	MPI_Comm Comm = MPI_COMM_WORLD;
	MPI_Status status;

	int
		my_id, //текущий id
		root_process, //id рута
		ierr,
		num_procs, //кол-во процессов
		an_id,
		avg_rows_per_process, //средний размер массива для каждого процесса
		start_row, //начало отчета массива
		end_row; //конец отчета массива

	root_process = 0;

	///MPI инициализация
	ierr = MPI_Init(&argc, &argv);

	ierr = MPI_Comm_rank(Comm, &my_id);
	ierr = MPI_Comm_size(Comm, &num_procs);

	MPI_Barrier(MPI_COMM_WORLD);

	///Код основного корневого процесса
	if (my_id == root_process) {

		//Если есть входные аргументы: путь к массиву и число для нахождения суммы
		//То используем их иначе берем дефолтные
		if (argc > 1) {
			arrayPath = argv[1];
			inputNumber = atoi(argv[2]);
			numberArray = getNumberElements(argv[1]);
		}
		else {
			inputNumber = 6;
			arrayPath = (char*)R"(D:\Magistratura\ClasterWork\1\Array.txt)";
			numberArray = getNumberElements(arrayPath);
		}

		// Количесвто возможных комбинаций элементов массива
		int variants = pow(2, numberArray);

		auto* array = new int[numberArray]; // Наш массив
		auto* powerOfTwo = new int[numberArray]; // Массив степеней двойки
		int* possibilities;


		for (int i = 0; i < numberArray; i++) {
			powerOfTwo[i] = pow(2, i);
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

		// Вычисляем размер области по которой должен пройтись каждый процесс
		avg_rows_per_process = variants / num_procs;

		// Передаем ключевые значения всем процессам
		MPI_Bcast(&avg_rows_per_process, 1, MPI_INT, 0, Comm); 
		MPI_Bcast(&inputNumber, 1, MPI_INT, 0, Comm); 
		MPI_Bcast(&numberArray, 1, MPI_INT, 0, Comm);
		MPI_Bcast(array, numberArray, MPI_INT, 0, Comm);
		MPI_Bcast(powerOfTwo, numberArray, MPI_INT, 0, Comm);

		//Вычисляем для своей оласти
		calculate(
			1,
			avg_rows_per_process,
			inputNumber,
			numberArray,
			avg_rows_per_process,
			array,
			powerOfTwo);

		delete[] array;
		delete[] powerOfTwo;

	}
	else
	{
		//Получаем ключевые параметры
		MPI_Bcast(&avg_rows_per_process, 1, MPI_INT, 0, Comm);
		MPI_Bcast(&inputNumber, 1, MPI_INT, 0, Comm);
		MPI_Bcast(&numberArray, 1, MPI_INT, 0, Comm);

		int* power_of_two_to_recieve = new int[numberArray];
		int* array_to_recieve = new int[numberArray];

		MPI_Bcast(array_to_recieve, numberArray, MPI_INT, 0, Comm);
		MPI_Bcast(power_of_two_to_recieve, numberArray, MPI_INT, 0, Comm);

		// Конкретизируем область работы
		start_row = avg_rows_per_process * my_id;
		end_row = avg_rows_per_process * (my_id + 1);

		calculate(
			start_row,
			end_row,
			inputNumber,
			numberArray,
			avg_rows_per_process,
			array_to_recieve,
			power_of_two_to_recieve);
	}

	ierr = MPI_Finalize();
	return 0;
}

///Вычислить маску массива
void calculate(int start_row, int end_row, int inputNumber, int numberArray, int variants, int* array, int* powerOfTwo) {

	auto ku = powerOfTwo[0];
	int s = 0;
	int posCounter = 0;
	auto* possibilities = new int[variants];

	/// Для числа области подбираем двоичную маску, отвечающую за использование числа из массива чисел
	for (int i = start_row; i < end_row + 1; i++) {
		for (int j = 0; j < numberArray; j++) {
			if ((i & powerOfTwo[j]) != 0) {
				s += array[j];
			}
		}

		///Если сумма по полученой маске равна искомой - выводим эту маску
		if (inputNumber == s) {
			getMask(i, numberArray, powerOfTwo);
		}

		s = 0;
	}
}

///Получить из файла количество элементов массива
int getNumberElements(const char* path) {
	ifstream in(path);
	string str;
	int temp = 0;
	if (in.is_open()) {
		while (getline(in, str)) {
			temp++;
		}
	}
	else cout << "Error reading" << endl;
	in.close();
	return temp;
}

///Вывести маску массива
void getMask(int numberToMask, int numberArray, int* powerOfTwo) {
	cout << "Mask: ";
	for (int j = 0; j < numberArray; j++) {
		if ((numberToMask & powerOfTwo[j]) != 0) {
			cout << "1";
		}
		else {
			cout << "0";
		}
	}
	cout << "\n";
}
