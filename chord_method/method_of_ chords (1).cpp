#include <iostream>
#include <cmath>

// Определение функции, для которой ищем корень
double f(double x) {
    // Пример функции: f(x) = x^3 - 2x - 5
    return pow(x, 3) - 2 * x - 5;
}

// Реализация метода хорд
double chordMethod(double (*func)(double), double x0, double x1, double epsilon=1e-6, int max_iter=100) {
    double f_x0 = func(x0);
    double f_x1 = func(x1);

    for (int i = 0; i < max_iter; ++i) {
        if (fabs(f_x1 - f_x0) < 1e-14) {
            std::cerr << "Разделитель близок к нулю. Метод может не сойтись." << std::endl;
            break;
        }

        // Вычисляем следующую точку по формуле метода хорд
        double x2 = x1 - f_x1 * (x1 - x0) / (f_x1 - f_x0);

        // Проверка условия сходимости
        if (fabs(func(x2)) < epsilon || fabs(x2 - x1) < epsilon) {
            return x2;
        }

        // Обновляем приближения
        x0 = x1;
        f_x0 = f_x1;
        x1 = x2;
        f_x1 = func(x1);
    }
    return x1; // Возвращаем последнее приближение
}

int main() {
    // Начальные приближения
    double x0 = 2.0;
    double x1 = 3.0;

    double root = chordMethod(f, x0, x1);

    std::cout << "Приблизительный корень: " << root << std::endl;
    std::cout << "Значение функции в корне: " << f(root) << std::endl;

    return 0;
}