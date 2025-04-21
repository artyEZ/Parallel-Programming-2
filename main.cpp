#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <filesystem>
#include <sstream>
#include <omp.h>

const std::string SAVE_PATH = "C:/Users/artyo/Desktop/Учеба/Параллельное программирование/Lab2";

std::vector<std::vector<int>> createSquareMatrix(int size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 99);

    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = distrib(gen);
        }
    }
    return matrix;
}

void writeMatrixToFile(const std::vector<std::vector<int>>& matrix, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filepath << std::endl;
        exit(1);
    }

    int size = matrix.size();
    file << size << std::endl;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            file << matrix[i][j] << " ";
        }
        file << std::endl;
    }
    file.close();
}

// Функция для умножения матриц с применением OpenMP
std::vector<std::vector<int>> multiplyMatrices(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B) {
    int rowsA = A.size();
    int colsA = A[0].size();
    int colsB = B[0].size();

    std::vector<std::vector<int>> result(rowsA, std::vector<int>(colsB, 0));

#pragma omp parallel for
    for (int i = 0; i < rowsA; ++i) {
        for (int k = 0; k < colsA; ++k) {
            int a_ik = A[i][k];
            for (int j = 0; j < colsB; ++j) {
                result[i][j] += a_ik * B[k][j];
            }
        }
    }

    return result;
}

int main() {
    std::vector<int> sizes = {10,20,50,100,200,500,1000};
    int num_runs = 1; 
    int max_threads = omp_get_max_threads(); // Максимальное число потоков

    std::ofstream resultsFile(SAVE_PATH + "/result_OpenMP.csv");
    resultsFile << "Size,Threads,C++ Time (seconds)\n";

    for (int size : sizes) {
        double total_time = 0.0;

        for (int run = 0; run < num_runs; ++run) {
            std::vector<std::vector<int>> matrixA = createSquareMatrix(size);
            std::vector<std::vector<int>> matrixB = createSquareMatrix(size);

            auto start = std::chrono::high_resolution_clock::now();
            std::vector<std::vector<int>> result = multiplyMatrices(matrixA, matrixB);
            auto end = std::chrono::high_resolution_clock::now();
            total_time += std::chrono::duration<double>(end - start).count();

            if (run == 0) { 
                std::ostringstream oss;
                oss << SAVE_PATH << "/matrixA_size_" << size << ".txt";
                writeMatrixToFile(matrixA, oss.str());

                oss.str("");
                oss << SAVE_PATH << "/matrixB_size_" << size << ".txt";
                writeMatrixToFile(matrixB, oss.str());

                oss.str("");
                oss << SAVE_PATH << "/result_size_" << size << ".txt";
                writeMatrixToFile(result, oss.str());
            }
        }

        double average_time = total_time / num_runs;
        resultsFile << size << "," << average_time << ",\n"; 
        std::cout << "Matrix size: " << size << "x" << size << " - Average C++ time: " << average_time << " seconds\n";
    }

    resultsFile.close();
    return 0;
}