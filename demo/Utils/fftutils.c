/*
 * fftutil.c
 *
 *  Created on: Jan 5, 2018
 *      Author: deanm
 */

#include "Adafruit_ZeroFFT.h"
#include "math.h"
/*
 * @brief  In-place bit reversal function.
 * @param[in, out] *pSrc        points to the in-place buffer of Q15 data type.
 * @param[in]      fftLen       length of the FFT.
 * @param[in]      bitRevFactor bit reversal modifier that supports different
 * size FFTs with the same bit reversal table
 * @param[in]      *pBitRevTab  points to bit reversal table.
 * @return none.
 */

static q15_t ALIGN4 scratchData[ZERO_FFT_MAX];

void arm_bitreversal_q15(q15_t *pSrc16, uint32_t fftLen, uint16_t bitRevFactor,
                         uint16_t *pBitRevTab) {
  q31_t *pSrc = (q31_t *)pSrc16;
  q31_t in;
  uint32_t fftLenBy2, fftLenBy2p1;
  uint16_t i, j;

  /*  Initializations */
  j = 0u;
  fftLenBy2 = fftLen / 2u;
  fftLenBy2p1 = (fftLen / 2u) + 1u;

  /* Bit Reversal Implementation */
  for (i = 0u; i <= (fftLenBy2 - 2u); i += 2u) {
    if (i < j) {
      /*  pSrc[i] <-> pSrc[j]; */
      /*  pSrc[i+1u] <-> pSrc[j+1u] */
      in = pSrc[i];
      pSrc[i] = pSrc[j];
      pSrc[j] = in;

      /*  pSrc[i + fftLenBy2p1] <-> pSrc[j + fftLenBy2p1];  */
      /*  pSrc[i + fftLenBy2p1+1u] <-> pSrc[j + fftLenBy2p1+1u] */
      in = pSrc[i + fftLenBy2p1];
      pSrc[i + fftLenBy2p1] = pSrc[j + fftLenBy2p1];
      pSrc[j + fftLenBy2p1] = in;
    }

    /*  pSrc[i+1u] <-> pSrc[j+fftLenBy2];         */
    /*  pSrc[i+2] <-> pSrc[j+fftLenBy2+1u]  */
    in = pSrc[i + 1u];
    pSrc[i + 1u] = pSrc[j + fftLenBy2];
    pSrc[j + fftLenBy2] = in;

    /*  Reading the index for the bit reversal */
    j = *pBitRevTab;

    /*  Updating the bit reversal index depending on the fft length  */
    pBitRevTab += bitRevFactor;
  }
}

void arm_radix2_butterfly_q15(q15_t *pSrc, uint32_t fftLen, q15_t *pCoef,
                              uint16_t twidCoefModifier) {
  uint16_t i, j, k, l;
  uint16_t n1, n2, ia;
  q15_t xt, yt, cosVal, sinVal;

  // N = fftLen;
  n2 = fftLen;

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  // loop for groups
  for (j = 0; j < n2; j++) {
    cosVal = pCoef[ia * 2];
    sinVal = pCoef[(ia * 2) + 1];
    ia = ia + twidCoefModifier;

    // loop for butterfly
    for (i = j; i < fftLen; i += n1) {
      l = i + n2;
      xt = (pSrc[2 * i] >> 2u) - (pSrc[2 * l] >> 2u);
      pSrc[2 * i] = ((pSrc[2 * i] >> 2u) + (pSrc[2 * l] >> 2u)) >> 1u;

      yt = (pSrc[2 * i + 1] >> 2u) - (pSrc[2 * l + 1] >> 2u);
      pSrc[2 * i + 1] =
          ((pSrc[2 * l + 1] >> 2u) + (pSrc[2 * i + 1] >> 2u)) >> 1u;

      pSrc[2u * l] = (((int16_t)(((q31_t)xt * cosVal) >> 16)) +
                      ((int16_t)(((q31_t)yt * sinVal) >> 16)));

      pSrc[2u * l + 1u] = (((int16_t)(((q31_t)yt * cosVal) >> 16)) -
                           ((int16_t)(((q31_t)xt * sinVal) >> 16)));

    } // butterfly loop end

  } // groups loop end

  twidCoefModifier = twidCoefModifier << 1u;

  // loop for stage
  for (k = fftLen / 2; k > 2; k = k >> 1) {
    n1 = n2;
    n2 = n2 >> 1;
    ia = 0;

    // loop for groups
    for (j = 0; j < n2; j++) {
      cosVal = pCoef[ia * 2];
      sinVal = pCoef[(ia * 2) + 1];
      ia = ia + twidCoefModifier;

      // loop for butterfly
      for (i = j; i < fftLen; i += n1) {
        l = i + n2;
        xt = pSrc[2 * i] - pSrc[2 * l];
        pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]) >> 1u;

        yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
        pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]) >> 1u;

        pSrc[2u * l] = (((int16_t)(((q31_t)xt * cosVal) >> 16)) +
                        ((int16_t)(((q31_t)yt * sinVal) >> 16)));

        pSrc[2u * l + 1u] = (((int16_t)(((q31_t)yt * cosVal) >> 16)) -
                             ((int16_t)(((q31_t)xt * sinVal) >> 16)));

      } // butterfly loop end

    } // groups loop end

    twidCoefModifier = twidCoefModifier << 1u;
  } // stages loop end

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  // loop for groups
  for (j = 0; j < n2; j++) {
    cosVal = pCoef[ia * 2];
    sinVal = pCoef[(ia * 2) + 1];

    ia = ia + twidCoefModifier;

    // loop for butterfly
    for (i = j; i < fftLen; i += n1) {
      l = i + n2;
      xt = pSrc[2 * i] - pSrc[2 * l];
      pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]);

      yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
      pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]);

      pSrc[2u * l] = xt;

      pSrc[2u * l + 1u] = yt;

    } // butterfly loop end

  } // groups loop end

  twidCoefModifier = twidCoefModifier << 1u;
}

static inline void applyWindow(q15_t *src, const q15_t *window, uint16_t len) {
  while (len--) {
    int32_t val = *src * *window++;
    *src++ = val >> 15;
  }
}

int ZeroFFT(q15_t *source, uint16_t length) {
  uint16_t twidCoefModifier;
  uint16_t bitRevFactor;
  uint16_t *pBitRevTable;
  q15_t *pSrc = source;
  q15_t *pOut = scratchData;
  q15_t real;
  q15_t img;
  uint16_t i;

  /*  Initializations of structure parameters for 256 point FFT */
  twidCoefModifier = 1u;
  bitRevFactor = 1u;
  pBitRevTable = (uint16_t *)&armBitRevTable[0];

  applyWindow(source, window_hanning_256, length);

  // split the data

  for (i = 0; i < length; i++) {
    *pOut++ = *pSrc++; // real
    *pOut++ = 0;       // imaginary
  }

  arm_radix2_butterfly_q15(scratchData, length, (q15_t *)twiddleCoefQ15, twidCoefModifier);
  arm_bitreversal_q15(scratchData, length, bitRevFactor, pBitRevTable);

  pSrc = source;
  pOut = scratchData;
  for (i = 0; i < length; i++) {
    real = *pOut++;
    img = *pOut++;
    //*pSrc++ = val > 0 ? val : -val;
    *pSrc++ = sqrt((int32_t)real * real + (int32_t)img * img);
    //pOut++; // discard imaginary phase val
  }
  source[0] /= 2;

  return 0;
}
