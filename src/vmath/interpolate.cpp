#include "interpolate.h"

///@file vmath/interpolate.cpp Interpolation support. @ingroup vmath

float bernstein(float u, int i, int d) {
    if(i < 0 or i > d) return 0;
    if(d == 0) return 1;
    else if(d == 1) {
        if(i == 0) return 1-u;
        else if(i == 1) return u;
    } else if(d == 2) {
        if(i == 0) return (1-u)*(1-u);
        else if(i == 1) return 2*u*(1-u);
        else if(i == 2) return u*u;
    } else if(d == 3) {
        if(i == 0) return (1-u)*(1-u)*(1-u);
        else if(i == 1) return 3*u*(1-u)*(1-u);
        else if(i == 2) return 3*u*u*(1-u);
        else if(i == 3) return u*u*u;
    } else {
        return (1-u)*bernstein(u, i, d-1) + u*bernstein(u, i-1, d-1);
    }
    return 0;
}

float bernstein_derivative(float u, int i, int d) {
    return d * (bernstein(u, i-1, d-1) - bernstein(u, i, d-1));
}
