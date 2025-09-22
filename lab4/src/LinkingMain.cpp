#include <iostream>
#include <windows.h>
#include "Integral.h"
#include "Eps.h"
#include <string>

using namespace std;

int main(){

	cout << "Программа join на этапе линковки" << endl << endl;

	unsigned int switch_ = 1;

	while(1) {

		cout << "Введите название библиотеки, которую хотите подключить: \n 'integral' - Интеграл \n 'eps' - Вычисление значения натурального алгоритма \n"  << endl;
		cout << "Введите Y, если хотите переключить реализацию расчёта \n Изначальная реализация - 1" << endl;
		cout << "Введите 'exit' если хотети выйти из программы " << endl;

		string lib;
		string indicator;

		if (!(cin >> lib) || !(cin >> indicator)) {
			cout << "Заверешние работы программы" << endl;
			system("pause");
			break;
		}
		else if (indicator == "Y") {
			if (switch_ == 1) {
				switch_ = 2;
			}
			else {
				switch_ = 1;
			}
		cout << "Реализация переключена на " << switch_ << endl;
		}
		else if (lib == "integral") {

			float a, b, eps;

			if (switch_ == 1) {
				cout << "Введите границы отрезка интегрирования (a, b): " << endl;
				cin >> a >> b;

				cout << "Введите эпсилон: " << endl;
				cin >> eps;

				float integral = IntegrateFirst(a, b, eps);
				cout << "Ответ: " << integral << " - интеграл решен методом прямоугольников" << endl;
				system("pause");
			}
			else {
				cout << "Введите границы отрезка интегрирования (a, b): " << endl;
				cin >> a >> b;

				cout << "Введите эпсилон: " << endl;
				cin >> eps;

				float integral = IntegrateSecond(a, b, eps);
				cout << "Ответ: " << integral << " - интеграл решен методом трапеций" << endl;
				system("pause");
			}
		}

		else if (lib == "eps") {

			int x;

			if (switch_ == 1) {
				cout << "Введите значения числа x (int)" << endl;
				cin >> x;

				float eps = EpsFirst(x);
				cout << "E = " << eps << "- решен методом (1+1/x)^x" << endl;
				system("pause");
			}
			else {
				cout << "Введите значение числа x (int) " << endl;
				cin >> x;

				float eps = EpsSecond(x);
				cout << "E = " << eps << "- решен методом суммой ряда по n от 0 до x, где элементы равны: (1/(n!)) " << endl;
				system("pause");
			}
		}
		else {
			cout << "Нет решения, так как не выбрана библиотека функции" << endl;
			system("pause");
			break;
		}
	}

	return 0;
}