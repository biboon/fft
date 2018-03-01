#include "radix2.h"
#include "radix4.h"
#include "radix22.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846	/* pi */
#endif

static void benchFFT(complex* input, complex* output, complex* twids, int n, int numb, int mode);
static void gen_twiddles(complex *W, int size);
static int reverse(int n, int length);


static struct {
    char *name;
    void (*fft)(complex *input, complex *output, complex *twids, int samples);
} algorithms[] = {
    { "radix-2",  radix2_DIF  },
    { "radix-4",  radix4_DIF  },
    { "radix-22", radix22_DIF },
};


#if 0
/* DFT algorithm */
void DFT(Complex* input, Complex* output, Complex* twids, int samples) {
    int k, n;
    Complex prod;
    Complex* F_k;
    // Set output array to 0 first
    memset(output, 0, samples * sizeof(Complex));

    for (k = 0; k < samples; k++) {
        for (n = 0; n < samples; n++) {
            prod = cmult(*(twids + (k*n)%samples), *(input + n));
            F_k = output + n;
            F_k->r += prod.r;
            F_k->i += prod.i;
        }
    }
}
#endif


int main(int argc, char** argv)
{
    int n, ffts, mode;
    int k, _k, log2n;

    if (argc != 4)
    {
        printf("Usage: %s <samples number> <number of FFTs> <mode>\n", argv[0]);
        return 1;
    }

    sscanf(argv[1], "%d", &n);
    sscanf(argv[2], "%d", &ffts);
    sscanf(argv[3], "%d", &mode);

    log2n = (int) log2(n);

    // Allocate memory
    complex* input = (complex*) calloc(n, sizeof(complex));
    complex* output = (complex*) calloc(n, sizeof(complex));
    complex* twids = (complex*) calloc(3 * n / 4, sizeof(complex));

    // Set signal in input
    for (k = 0; k < n; k++) input[k] = sin(2 * M_PI * k / 128);

    // Set the twiddle factors
    gen_twiddles(twids, n);

    // Do the FFT
    benchFFT(input, output, twids, n, ffts, mode);

    // Output module to the file
    FILE *fd = fopen("topkek.txt", "w+");
    for (k = 0; k < n; k++) {
        _k = reverse(k, log2n);
        fprintf(fd, "%f\n", cabs(output[_k]));
    }
    fclose(fd);

    free(input);
    free(output);
    free(twids);

    return 0;
}


void benchFFT(complex* input, complex* output, complex* twids, int n, int numb, int mode)
{
    int k;
    clock_t begin, end;
    double seconds;

    printf("Benchmarking algorithms for %u x %u-FFTs...\n", numb, n);

    for (int i = 0; i < sizeof(algorithms) / sizeof(algorithms[0]); ++i)
    {
        if (!(mode & (1 << i))) continue;

        begin = clock();
        for (k = numb; k != 0; k--)
            algorithms[i].fft(input, output, twids, n);
        end = clock();

        seconds = (double)(end - begin) / CLOCKS_PER_SEC;
        printf(" * %s\n", algorithms[i].name);
        printf(" |  %f us/FFT\n", seconds * 1e6 / numb);
        printf(" |  %f FFTs/s\n", numb / seconds);
        printf(" |  %f MSPS\n", n * numb / seconds / 1e6);
    }

#if 0
    if (mode & 0x8) {
        begin = clock();
        for (k = numb; k != 0; k--) DFT(input, output, twids, n);
        end = clock();
        seconds = (double)(end - begin)/CLOCKS_PER_SEC;
        printf(" DFT      : %f us\n", seconds * 1000000 / numb);
    }
#endif
}


void gen_twiddles(complex *W, int size)
{
    for (int k = 0; k < size / 4; k++)
    {
        double a = -(k * 2 * M_PI / size);
        complex z = cos(a) + I * sin(a);

        W[k           ] = z;
        W[k + size / 4] = z * (-I);
        W[k + size / 2] = z * (-1);
    }
}

int reverse(int n, int length)
{
    int res = 0;

    for (int i = 0; i < length; i++)
    {
        res |= (n & 0x1) << (length - i - 1);
        n >>= 1;
    }

    return res;
}