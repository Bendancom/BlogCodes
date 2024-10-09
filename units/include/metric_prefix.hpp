#ifndef METRIC_PREFIX
#define METRIC_PREFIX

#include <array>

enum class MP : char{
    Q,
    R,
    Y,
    Z,
    E,
    P,
    T,
    G,
    M,
    k,
    h,
    da,
    null,
    d,
    c,
    m,
    μ,
    n,
    p,
    f,
    a,
    z,
    y,
    r,
    q,
};

auto MP_array = std::to_array<float>({
    1e30,
    1e27,
    1e24,
    1e21,
    1e18,
    1e15,
    1e12,
    1e9,
    1e6,
    1e3,
    1e2,
    1e1,
    1,
    1e-1,
    1e-2,
    1e-3,
    1e-6,
    1e-9,
    1e-12,
    1e-15,
    1e-18,
    1e-21,
    1e-24,
    1e-27,
    1e-30
});
#endif
