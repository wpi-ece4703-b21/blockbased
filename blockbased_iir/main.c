#include "xlaudio.h"

#define BLOCKSIZE 8
#define NUMSECTIONS 2
float32_t taps[4 * NUMSECTIONS];
float32_t coefficients[5 * NUMSECTIONS] = {1.0f, 0.0f, 1.0f, -0.707f, 0.25f,
                                           1.0f, 1.0f, 0.0f,  0.707f, 0.25f};
arm_biquad_casd_df1_inst_f32 F;

void processBuffer(uint16_t x[BLOCKSIZE], uint16_t y[BLOCKSIZE]) {
    float32_t xf[BLOCKSIZE], yf[BLOCKSIZE];
    xlaudio_adc14_to_f32_vec(x, xf, BLOCKSIZE);
    arm_biquad_cascade_df1_f32(&F, xf, yf, BLOCKSIZE);
    xlaudio_f32_to_dac14_vec(yf, y, BLOCKSIZE);
}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    arm_biquad_cascade_df1_init_f32(&F, NUMSECTIONS, coefficients, taps);

    xlaudio_init_dma(FS_8000_HZ, XLAUDIO_J1_2_IN, BUFLEN_8, processBuffer);

    xlaudio_run();

    return 1;
}
