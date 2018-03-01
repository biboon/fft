#include "radix2.h"

#include <string.h>


static void butterfly(complex *output, complex *twids, int n, int local_size, int samples);


void radix2_DIF(complex *input, complex *output, complex *twids, int samples)
{
    memcpy(output, input, samples * sizeof(complex));

    for (int local_size = samples; local_size > 1; local_size /= 2)
        for (int n = 0; n < samples / 2; n++)
            butterfly(output, twids, n, local_size, samples);
}


/* Radix 2 algorithm */
void butterfly(complex *output, complex *twids, int n, int local_size, int samples)
{
    int twidindex, q, r, _n;
    complex stage[2][2];

    // Do the Euclidian division of n
    q = n / (local_size / 2);
    r = n % (local_size / 2);
    _n = q * local_size + r;
    twidindex = r * samples / local_size;

    /* Init values */
    stage[0][0] = output[_n + 0 * local_size / 2];
    stage[0][1] = output[_n + 1 * local_size / 2];

    /* Process first stage of butterfly */
    stage[1][0] = stage[0][0] + stage[0][1];
    stage[1][1] = stage[0][0] - stage[0][1];

    /* Multiply with twiddle factors */
    output[_n + 0 * local_size / 2] = stage[1][0];
    output[_n + 1 * local_size / 2] = stage[1][1] * twids[twidindex];
}
