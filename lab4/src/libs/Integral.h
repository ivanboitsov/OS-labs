// Рассчет интеграла функции sin(x) на отрезке[A, B] с шагом e
#pragma once
extern "C" {
    __declspec(dllexport) double IntegrateFirst(double a, double b, double eps);

    __declspec(dllexport) double IntegrateSecond(double a, double b, double eps);
}