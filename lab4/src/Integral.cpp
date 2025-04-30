#include <iostream>
#include <cmath>
#include "Integral.h"

using namespace std;
extern "C"{
	// Подсчёт интеграла методом прямоугольников
	__declspec(dllexport) double IntegrateFirst(double a, double b, double eps) {
		
		int steps = fabs(b - a) / eps;
		double point = a;
		double result = 0;

		for (int i = 0; i < steps; i++) {
			result += sin(point) * eps;
			point += eps;
		}
		
		return result;
	}

	// Подсчёт интерграла методом трапеции
	__declspec(dllexport) double IntegrateSecond(float a, float b, float eps) {

		int steps = (b - a) / eps;
		double point = a;
		double result = 0;

		for (int i = 0; i < steps; i++) {
			result += sin(point + eps / 2) * eps;
			point += eps;
		}

		return result;
	}
}