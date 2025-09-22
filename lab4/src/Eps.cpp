#include <iostream>
#include <cmath>
#include "Eps.h"

using namespace std;

extern "C" {
	// Вычисление факториала
	long long fact(int n) {
        long long result = 1;
        for (int i = 1; i <= n; i++) {
            result *= i;
        }
        return result;
    }

	// (1 + 1/x) ^ x
	__declspec(dllexport) float EpsFirst(int x) {
		return pow((1 + 1.0 / (float)x), x);
	}

	// Сумма ряда по n от 0 до x, где элементы ряда равны: (1/(n!))
	__declspec(dllexport) float EpsSecond(int x) {

		float to_return = 0;
		for (int n = 0; n < x; n++) {
			to_return = ((float)1 / fact(n));
		}
		
		return to_return;
	}
}