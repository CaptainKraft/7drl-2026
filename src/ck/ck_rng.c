#include "ck/ck_rng.h"

uint64_t ck_rand(RNG *rng)
{
    rng->seed += 0x60bee2bee120fc15;
    __uint128_t tmp;
    tmp = (__uint128_t)rng->seed * 0xa3b195354a39b70d;
    uint64_t m1 = (tmp >> 64) ^ tmp;
    tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
    uint64_t m2 = (tmp >> 64) ^ tmp;
    return m2;
}

int ck_rand_int(RNG *rng, int min, int max)
{
    return (ck_rand(rng) % (max - min)) + min;
}

float ck_rand_float(RNG *rng, float min, float max)
{
    // Use upper 32 bits for better float precision
    uint64_t rand_val = ck_rand(rng) >> 32;
    return min + (rand_val / (float)UINT32_MAX) * (max - min);
}

RNG ck_rng_fork(const RNG *base, uint64_t stream_id)
{
    RNG forked = {.seed = base->seed ^ ((__uint128_t)stream_id << 64) ^ stream_id};
    (void)ck_rand(&forked);
    return forked;
}
