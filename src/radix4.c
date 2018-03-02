#include "radix4.h"

#include <string.h>


static void butterfly(complex *output, complex *twids, int sub_size, int bf, int size);


void radix4_DIF(complex *input, complex *output, complex *twids, int size)
{
	memcpy(output, input, size * sizeof(complex));

	for (int sub_size = size; sub_size > 1; sub_size /= 4)
		for (int sub_dft = 0; sub_dft < size / sub_size; ++sub_dft)
			for (int bf = 0; bf < sub_size / 4; ++bf)
				butterfly(&output[sub_dft * sub_size], twids, sub_size, bf, size);
}


/* Radix 4 algorithm */
void butterfly(complex *output, complex *twids, int sub_size, int bf, int size)
{
	complex stage[2][4];
	int twidindex = bf * size / sub_size;

	/* Init values */
	stage[0][0] = output[bf + 0 * sub_size / 4];
	stage[0][1] = output[bf + 1 * sub_size / 4];
	stage[0][2] = output[bf + 2 * sub_size / 4];
	stage[0][3] = output[bf + 3 * sub_size / 4];

	/* Process first stage of butterfly */
	stage[1][0] = stage[0][0] +     stage[0][1] +     stage[0][2] +     stage[0][3];
	stage[1][1] = stage[0][0] - I * stage[0][1] -     stage[0][2] + I * stage[0][3];
	stage[1][2] = stage[0][0] -     stage[0][1] +     stage[0][2] -     stage[0][3];
	stage[1][3] = stage[0][0] + I * stage[0][1] -     stage[0][2] - I * stage[0][3];

	/* Multiply with twiddle factors */
	output[bf + 0 * sub_size / 4] = stage[1][0];
	output[bf + 1 * sub_size / 4] = stage[1][1] * twids[1 * twidindex];
	output[bf + 2 * sub_size / 4] = stage[1][2] * twids[2 * twidindex];
	output[bf + 3 * sub_size / 4] = stage[1][3] * twids[3 * twidindex];
}
