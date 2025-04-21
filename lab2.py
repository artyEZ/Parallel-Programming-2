import numpy as np
import matplotlib.pyplot as plt
import time
import os
import csv


def read_matrix_from_file(filepath):
    with open(filepath, 'r') as file:
        size = int(file.readline())
        matrix = np.loadtxt(file, dtype=int, max_rows=size)
    return matrix


def verify_results(cpp_result, python_result):
    return np.allclose(cpp_result, python_result, atol=1e-6)


if __name__ == "__main__":
    save_path = "C:/Users/artyo/Desktop/Учеба/Параллельное программирование/Lab2"
    sizes = [10, 50, 100, 200, 500, 1000]
    results = []

    for size in sizes:
        matrixA_path = os.path.join(save_path, f"matrixA_size_{size}.txt")
        matrixB_path = os.path.join(save_path, f"matrixB_size_{size}.txt")
        result_path = os.path.join(save_path, f"result_size_{size}.txt")

        matrixA = read_matrix_from_file(matrixA_path)
        matrixB = read_matrix_from_file(matrixB_path)
        cpp_result = read_matrix_from_file(result_path)

        start = time.time()
        python_result = np.dot(matrixA, matrixB)
        end = time.time()
        python_time = end - start

        if verify_results(cpp_result, python_result):
            print(f"Verification for size {size}x{size}: PASSED")
        else:
            print(f"Verification for size {size}x{size}: FAILED")

        results.append((size, python_time))

    # Запись времени выполнения Python в CSV
    with open(os.path.join(save_path, "results_OpenMP.csv"), 'r') as file:
        lines = list(csv.reader(file))
    with open(os.path.join(save_path, "results_OpenMP.csv"), 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(lines[0])  # Заголовок
        for i, (size, python_time) in enumerate(results):
            writer.writerow([size, lines[i + 1][1], python_time])

    # Построение графика
    sizes, cpp_times, python_times = [], [], []
    with open(os.path.join(save_path, "results_OpenMP.csv"), 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Пропуск заголовка
        for row in reader:
            sizes.append(int(row[0]))
            cpp_times.append(float(row[1]))
            python_times.append(float(row[2]))

    plt.plot(sizes, cpp_times, label='OpenMP', marker='o')
    plt.plot(sizes, python_times, label='Python', marker='o')
    plt.xlabel('Matrix Size')
    plt.ylabel('Execution Time (seconds)')
    plt.title('Execution Time vs Matrix Size')
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(save_path, "execution_time_comparison.png"))
    plt.show()