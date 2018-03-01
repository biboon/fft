#include "radix4.h"

#include <string.h>


static void butterfly(complex *output, complex *twids, int n, int local_size, int samples);


void radix4_DIF(complex *input, complex *output, complex *twids, int samples)
{
    memcpy(output, input, samples * sizeof(complex));

    for (int local_size = samples; local_size > 1; local_size /= 4)
        for (int n = 0; n < samples / 4; n++)
            butterfly(output, twids, n, local_size, samples);
}


/* Radix 4 algorithm */
void butterfly(complex *output, complex *twids, int n, int local_size, int samples)
{
    int twidindex, q, r, _n;
    complex stage[2][4];

    // Do the Euclidian division of n
    q = n / (local_size / 4);
    r = n % (local_size / 4);
    _n = q * local_size + r;
    twidindex = r * samples / local_size;

    /* Init values */
    stage[0][0] = output[_n + 0 * local_size / 4];
    stage[0][1] = output[_n + 1 * local_size / 4];
    stage[0][2] = output[_n + 2 * local_size / 4];
    stage[0][3] = output[_n + 3 * local_size / 4];

    /* Process first stage of butterfly */
    stage[1][0] = stage[0][0] +     stage[0][1] +     stage[0][2] +     stage[0][3];
    stage[1][1] = stage[0][0] - I * stage[0][1] -     stage[0][2] + I * stage[0][3];
    stage[1][2] = stage[0][0] -     stage[0][1] +     stage[0][2] -     stage[0][3];
    stage[1][3] = stage[0][0] + I * stage[0][1] -     stage[0][2] - I * stage[0][3];

    /* Multiply with twiddle factors */
    output[_n + 0 * local_size / 4] = stage[1][0];
    output[_n + 1 * local_size / 4] = stage[1][1] * twids[1 * twidindex];
    output[_n + 2 * local_size / 4] = stage[1][2] * twids[2 * twidindex];
    output[_n + 3 * local_size / 4] = stage[1][3] * twids[3 * twidindex];
}
