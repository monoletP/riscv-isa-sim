#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float16_t f8_to_f16( float8_t a )
{
    //  Warning: assume f8 is E4M3FN

    union ui8_f8 uA;
    uint_fast8_t uiA;
    bool sign, isNaN;
    int_fast8_t exp;
    uint_fast8_t frac;
    // struct commonNaN commonNaN;
    uint_fast16_t uiZ;
    union ui16_f16 uZ;

    uA.f = a;
    uiA = uA.ui;
    sign = (bool) ((uiA & 0x80) >> 7);
    exp = (int_fast8_t) ((uiA >> 3) & 0x0F);
    frac = uiA & 0x07;
    isNaN = (exp == 0x0F) && (frac == 0x07);

    if(isNaN) {
        uiZ = defaultNaNF16UI;
        goto uiZ;
    } else {
        if(exp == 0) {
            // subnormal
            float16_t frac_f16 = ui32_to_f16((uint32_t) frac);
            float16_t divisor = { .v = 0x6000 }; //512.0

            float16_t magnitude = f16_div(frac_f16, divisor);

            uiZ = packToF16UI(sign, 0, magnitude.v);
            goto uiZ;
        }
        uiZ = packToF16UI(sign, exp + 8, (uint_fast16_t) frac << 7);
    }
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;
}