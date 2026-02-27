#pragma once

#include <stdint.h>

// Deterministic RNG utilities.
// - RNG holds a 128-bit state. Callers must seed it explicitly before use.
//   A common pattern is: rng->seed = ((__uint128_t)seed << 64) | seed;
// - Use separate RNG structs per purpose (sector gen, wormholes, upgrades) so
//   call order in one system cannot affect another.
// - If you want a derived stream without mutating a base RNG, use ck_rng_fork.
// Example:
//   RNG rng;
//   rng.seed = ((__uint128_t)seed << 64) | seed;
//   int roll = ck_rand_int(&rng, 0, 100);
typedef struct {
    __uint128_t seed;
} RNG;

// Return the next 64-bit random value and advance the RNG state.
// Prefer higher-level helpers unless you need raw bits.
uint64_t ck_rand(RNG *rng);

// Return an integer in the range [min, max) (max is exclusive). Requires max > min.
int ck_rand_int(RNG *rng, int min, int max);

// Return a float in the range [min, max]. Uses upper 32 bits for precision.
float ck_rand_float(RNG *rng, float min, float max);

// Derive a deterministic substream from a base RNG and stream id.
// The base RNG is not modified; the returned RNG is ready for use.
// Use stream_id to create stable, purpose-specific streams (e.g., per sector idx).
RNG ck_rng_fork(const RNG *base, uint64_t stream_id);
