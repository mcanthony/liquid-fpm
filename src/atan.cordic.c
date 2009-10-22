// 
// atan | cordic
// 

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_ATAN_CORDIC 1

// cordic coefficients
const q32_t q32_atan_cordic_Ak_tab[32] = {
    0x10000000,    0x09720290,    0x04fd9c28,    0x028888e8,
    0x014586a0,    0x00a2ebf0,    0x00517b0f,    0x0028be2b,
    0x00145f2a,    0x000a2f97,    0x000517cc,    0x00028be6,
    0x000145f3,    0x0000a2fa,    0x0000517d,    0x000028be,
    0x0000145f,    0x00000a30,    0x00000518,    0x0000028c,
    0x00000146,    0x000000a3,    0x00000051,    0x00000029,
    0x00000014,    0x0000000a,    0x00000005,    0x00000003,
    0x00000001,    0x00000001,    0x00000000,    0x00000000};

const q32_t q32_atan_cordic_k_inv = 0x09b74ee0;

void q32_atan2_cordic_base(q32_t _y,
                           q32_t _x,
                           q32_t * _r,
                           q32_t * _theta,
                           unsigned int _n);

void q32_atan2_cordic(q32_t _y,
                      q32_t _x,
                      q32_t * _r,
                      q32_t * _theta,
                      unsigned int _n)
{
#if 0
    // For all practical purposes we are observing r = y/x so the decimal
    // place in fixed point math can be ignored and we can treat y and x
    // as integers so long as we don't lose resolution

    // negate calculated angle if _y < 0
    int negate = (_y < 0);

    // subtract calculated angle from pi if _x < 0
    int sub_from_pi = (_x < 0);

    // subtract calculated angle from pi/2 if abs(y/x) < 1, or invert
    // y/x to by x/y so that log2 of ratio is positive
    int invert = 0;

    // take absolute value as we are only interested in absolute ratio
    // at this point.
    _y = abs(_y);
    _x = abs(_x);

    // compare x, y; place larger value in y, smaller in x, thus ratio
    // r = y/x is always greater than or equal to 1; keeps log2(y/x)
    // positive
    if (_y < _x) {
        // swap _x and _y values
        q32_t tmp = _x;
        _x = _y;
        _y = tmp;
        invert = 1;
    }
#endif

    q32_atan2_cordic_base(_y,_x,_r,_theta,_n);
}

void q32_atan2_cordic_base(q32_t _y,
                           q32_t _x,
                           q32_t * _r,
                           q32_t * _theta,
                           unsigned int _n)
{

    q32_t x = _x;
    q32_t y = _y;
    q32_t z = 0;
    q32_t d,tx,ty,tz;
    unsigned int i;
    unsigned int n=_n;

#if DEBUG_ATAN_CORDIC
    printf("   n            x            y            z        -d*An\n");
    printf("init %12.8f %12.8f %12.8f %12.8f\n",
            q32_fixed_to_float(x),
            q32_fixed_to_float(y),
            q32_fixed_to_float(z),
            0.0);
#endif
    for (i=0; i<n; i++) {
        d = ( y>=0 ) ? -1 : 0;

        tx = x - ((y>>i)^d)-d;
        ty = y + ((x>>i)^d)-d;
        tz = z - ((q32_atan_cordic_Ak_tab[i]^d)-d);
        x = tx;
        y = ty;
        z = tz;
#if DEBUG_ATAN_CORDIC
        printf("%4u %12.8f %12.8f %12.8f %12.8f\n",
            i,
            q32_fixed_to_float(x),
            q32_fixed_to_float(y),
            q32_fixed_to_float(z),
            q32_fixed_to_float(q32_atan_cordic_Ak_tab[i])*(y>=0?-1.0:1.0));
#endif
    }
    //q32_t x = q32_atan_cordic_k_inv; // TODO : initialize with cordic_Kinv_tab[_n-1];
    *_r = q32_mul(x,q32_atan_cordic_k_inv);
    *_theta = z;
}

