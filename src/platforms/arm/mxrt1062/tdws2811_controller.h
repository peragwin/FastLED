#ifndef __INC_TDWS2811_CONTROLLER_H
#define __INC_TDWS2811_CONTROLLER_H

#ifdef USE_TDWS2811

#include "TDWS2811.h"

FASTLED_NAMESPACE_BEGIN

/// Rotate 32 bytes into 8 words using four 8x8 bit-blocks
__attribute__((always_inline)) inline void transpose32x1_MSB(unsigned char *A, uint32_t *B) {
  unsigned char AT[32];
    
  for (int i = 0; i < 4; ++i) {
    transpose8x1_MSB(A+8*i, AT+8*i);
  }
  
  for (int i = 0; i < 8; ++i) {
      uint32_t b = 0;
      for (int j = 0; j < 4; ++j) {
        b |= AT[i+8*j] << 8*j;
      }
      *(B+i) = b;
  }
}

template <EOrder RGB_ORDER = GRB, int MAX_LEDS = 300>
class CTriantaduoWS2811Controller : public CPixelLEDController<RGB_ORDER, 32, 0xFFFFFFFF>
{
  TDWS2811<MAX_LEDS> tdws;

public:
  CTriantaduoWS2811Controller()
  {
    // byte ordering is handled by the pixel controller
    for (int i = 0; i < 32; ++i)
    {
      tdws.setChannelType(i, (typename TDWS2811<MAX_LEDS>::EOrder)RGB);
    }
  }

  virtual int size() { return CLEDController::size() * 32; }

  virtual void init()
  { /* do nothing yet */
  }

  virtual void showPixels(PixelController<RGB_ORDER, 32, 0xFFFFFFFF> &pixels)
  {
    if (pixels.size() > MAX_LEDS)
      return;

    uint32_t *pData = (uint32_t *)tdws.getInactiveBuffer();
    uint8_t b[32];

    while (pixels.has(1))
    {
      for (int i = 0; i < 32; ++i)
      {
        b[i] = pixels.loadAndScale0(i);
      }
      transpose32x1_MSB(b, pData);
      pData += 8;
      for (int i = 0; i < 32; ++i)
      {
        b[i] = pixels.loadAndScale1(i);
      }
      transpose32x1_MSB(b, pData);
      pData += 8;
      for (int i = 0; i < 32; ++i)
      {
        b[i] = pixels.loadAndScale2(i);
      }
      transpose32x1_MSB(b, pData);
      pData += 8;
      pixels.stepDithering();
      pixels.advanceData();
    }

    tdws.flipBuffers();
  }
};

FASTLED_NAMESPACE_END

#endif

#endif
