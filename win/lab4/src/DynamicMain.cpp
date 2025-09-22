#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

int main() {
    cout << "Программа join на этапе выполнения" << endl << endl;

    unsigned int switch_ = 1;

    while (true) {
        cout << "Введите название библиотеки, которую хотите подключить: \n'integral' - Интеграл \n'eps' - Вычисление значения натурального логарифма\n" << endl;
        cout << "Введите Y, если хотите переключить реализацию расчёта. Изначальная реализация - 1" << endl;
        cout << "Введите 'exit' если хотите выйти из программы" << endl;

        string lib;
        string indicator;

        if (!(cin >> lib) || !(cin >> indicator)) {
            cout << "Завершение работы программы" << endl;
            break;
        } else if (indicator == "Y") {
            switch_ = (switch_ == 1) ? 2 : 1;
            cout << "Реализация переключена на " << switch_ << endl;
        }

        if (lib == "integral") {

            HMODULE hLib = LoadLibrary(L"Integral.dll");
            if (!hLib) {
                cerr << "Не удалось загрузить Integral.dll" << endl;
                continue;
            }

            auto IntegrateFirst = (double (*)(double, double, double)) GetProcAddress(hLib, "IntegrateFirst");
            auto IntegrateSecond = (double (*)(double, double, double)) GetProcAddress(hLib, "IntegrateSecond");

            if (!IntegrateFirst || !IntegrateSecond) {
                cerr << "Ошибка загрузки функций из Integral.dll" << endl;
                FreeLibrary(hLib);
                continue;
            }

            double a, b, eps;
            cout << "Введите границы отрезка интегрирования (a, b): " << endl;
            cin >> a >> b;

            cout << "Введите эпсилон: " << endl;
            cin >> eps;

            double result = (switch_ == 1) ? IntegrateFirst(a, b, eps) : IntegrateSecond(a, b, eps);
            cout << "Ответ: " << result << " - интеграл решен методом "
                 << ((switch_ == 1) ? "прямоугольников" : "трапеций") << endl;


            FreeLibrary(hLib);

        } else if (lib == "eps") {

            HMODULE hLib = LoadLibrary(L"Eps.dll");
            if (!hLib) {
                cerr << "Не удалось загрузить Eps.dll" << endl;
                continue;
            }

            auto EpsFirst = (float (*)(int)) GetProcAddress(hLib, "EpsFirst");
            auto EpsSecond = (float (*)(int)) GetProcAddress(hLib, "EpsSecond");

            if (!EpsFirst || !EpsSecond) {
                cerr << "Ошибка загрузки функций из Eps.dll" << endl;
                FreeLibrary(hLib);
                continue;
            }

            int x;
            cout << "Введите значение числа x (int): ";
            cin >> x;

            float result = (switch_ == 1) ? EpsFirst(x) : EpsSecond(x);
            cout << "E = " << result << " - решен методом "
                 << ((switch_ == 1) ? "(1 + 1/x)^x" : "суммой ряда по n от 0 до x, где элементы равны: (1/(n!))") << endl;

            FreeLibrary(hLib);
        } else {
            cout << "Нет решения, так как не выбрана библиотека функции" << endl;
            break;
        }
    }

    return 0;
}