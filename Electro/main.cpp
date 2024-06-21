#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "eeg.h"


#define SAMPLE_RATE 500 // Частота выборки, 500 Гц
#define MAX_FREQ 40.0 // Максимальная частота сигнала, которую нужно оставить, 40 Гц
#define TWO_PI 6.283185307179586476925286766559

// Структура для комплексных чисел
typedef struct {
    double real;
    double imag;
} Complex;

// Функция для выполнения DFT
void DFT(int64_t* signal, Complex* result, int length) {
    for (int k = 0; k < length; k++) {
        result[k].real = 0.0;
        result[k].imag = 0.0;
        for (int n = 0; n < length; n++) {
            double angle = TWO_PI * k * n / length;
            result[k].real += signal[n] * cos(angle);
            result[k].imag -= signal[n] * sin(angle);
        }
    }
}

// Функция для выполнения IDFT
void IDFT(Complex* freq_domain, double* signal, int length) {
    for (int n = 0; n < length; n++) {
        signal[n] = 0.0;
        for (int k = 0; k < length; k++) {
            double angle = TWO_PI * k * n / length;
            signal[n] += freq_domain[k].real * cos(angle) - freq_domain[k].imag * sin(angle);
        }
        signal[n] /= length;
    }
}

// Функция для фильтрации сигнала
void filter_signal(int64_t* signal, int length) {
    Complex* freq_domain = (Complex*)malloc(sizeof(Complex) * length);
    double* filtered_signal = (double*)malloc(sizeof(double) * length);

    DFT(signal, freq_domain, length);

    // Фильтрация в частотной области
    for (int k = 0; k < length; k++) {
        double frequency = (double)k * SAMPLE_RATE / length;
        if (frequency > MAX_FREQ) {
            freq_domain[k].real = 0.0;
            freq_domain[k].imag = 0.0;
        }
    }

    IDFT(freq_domain, filtered_signal, length);

    // Копирование отфильтрованного сигнала обратно в целочисленный массив
    for (int i = 0; i < length; i++) {
        signal[i] = (int64_t)round(filtered_signal[i]);
    }

    free(freq_domain);
    free(filtered_signal);
}

int main() {
    int64_t pad[500] = {0};
    int64_t signal[500] = {0};

    const int length = 500;

    for(int i = 0; i < 10; i++){
        memcpy(pad, eeg_data + i*500, 500*sizeof(int64_t));
        memcpy(signal, eeg_data + i*500, 500*sizeof(int64_t));

        filter_signal(signal, length);
        for(int j = 0; j < 500; j++){
            //printf("%i\t", (i*500 + j));
            //printf("%lli\t%lli\n", pad[j], signal[j]);
            printf("%lli\n", signal[j]);
        }
       // printf("\n\n");
    }
    return 0;
}
