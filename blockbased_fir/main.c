#include "xlaudio.h"

#define BLOCKSIZE 8
#define NUMTAPS 32
q15_t taps[NUMTAPS + BLOCKSIZE - 1];
q15_t coefficients[NUMTAPS] = { (q15_t) (1 * (1 << 15)) };
arm_fir_instance_q15 F;

void processBuffer(uint16_t x[BLOCKSIZE], uint16_t y[BLOCKSIZE]) {
    q15_t xq[BLOCKSIZE], yq[BLOCKSIZE];
    xlaudio_adc14_to_q15_vec(x, xq, BLOCKSIZE);
    arm_fir_q15(&F, xq, yq, BLOCKSIZE);
    xlaudio_q15_to_dac14_vec(yq, y, BLOCKSIZE);
}

uint16_t processSample(uint16_t x) {
      int v = xlaudio_adc14_to_q15(x);
      taps[0] = v;
      int i, q = 0;
      for (i=0; i<NUMTAPS; i++)
          q = q + (taps[i] * coefficients[i]) >> 15;
      return xlaudio_q15_to_dac14(q);
}


#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    arm_fir_init_q15(&F, NUMTAPS, coefficients, taps, BLOCKSIZE);

    xlaudio_init_dma(FS_8000_HZ, XLAUDIO_J1_2_IN, BUFLEN_8, processBuffer);

    uint32_t c = xlaudio_measurePerfBuffer(processBuffer);
    printf("Cycles: %d\n", c);
     c = xlaudio_measurePerfSample(processSample);
    printf("Cycles: %d\n", c);

    xlaudio_run();

    return 1;
}

