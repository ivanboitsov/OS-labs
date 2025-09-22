// Рассчет значения числа е(основание натурального логарифма)
#pragma once
extern "C" {
    __declspec(dllexport) float EpsFirst(int x);

    __declspec(dllexport) float EpsSecond(int x);
}