//
//  math.cpp
//  toybox
//
//  Created by Fredrik on 2025-11-07.
//

#include "math.hpp"
#include "utility.hpp"
#include "array.hpp"

namespace toybox {

    fix16_t sqrt(fix16_t x) {
        if (x.raw <= 0) return fix16_t(0);
        // Initial guess - use magnitude-based heuristics
        int16_t guess_raw;
        if (x.raw >= (64 << 4)) {          // x >= 64
            guess_raw = x.raw >> 3;         // ≈ x/8 (works well for large x)
        } else if (x.raw >= (16 << 4)) {   // x >= 16
            guess_raw = x.raw >> 2;         // ≈ x/4
        } else if (x.raw >= (4 << 4)) {    // x >= 4
            guess_raw = x.raw >> 1;         // ≈ x/2
        } else if (x.raw >= (1 << 4)) {    // x >= 1
            guess_raw = x.raw;              // ≈ x
        } else {                            // x < 1
            guess_raw = (1 << 4);           // Guess 1.0
        }
        fix16_t guess(guess_raw, true);
        // Newton-Raphson: x_{n+1} = (x_n + a/x_n) / 2
        // Iterate up to 6 times with early termination on convergence
        for (int i = 0; i < 6; ++i) {
            fix16_t sum = guess + x.div(guess);
            fix16_t next = sum.div(2);  // Divide by 2
            if (next.raw == guess.raw) break;  // Converged
            guess = next;
        }
        return guess;
    }

    namespace detail {
        static constexpr float pi2 = 3.14159265358979323846f * 2.0f;
        static constexpr float pi = 3.14159265358979323846f;
        static constexpr float sin(float x) {
            // Normalize x to [0, 2π) range
            // Reduce to [0, 2π)
            while (x < 0.0f) x += pi2;
            while (x >= pi2) x -= pi2;
            // Use symmetry to reduce to [0, π]
            bool negate = false;
            if (x > pi) {
                x -= pi;
                negate = true;
            }
            // Taylor series: sin(x) = x - x³/3! + x⁵/5! - x⁷/7! + x⁹/9! - x¹¹/11!
            const float x2 = x * x;
            const float x3 = x2 * x;
            const float x5 = x3 * x2;
            const float x7 = x5 * x2;
            const float x9 = x7 * x2;
            const float x11 = x9 * x2;
            // Compute with factorials
            float result = x 
                         - x3 / 6.0f              // x³/3!
                         + x5 / 120.0f            // x⁵/5!
                         - x7 / 5040.0f           // x⁷/7!
                         + x9 / 362880.0f         // x⁹/9!
                         - x11 / 39916800.0f;     // x¹¹/11!
            return negate ? -result : result;
        }
        
        static consteval fix16_t generate_entry(int i) {
            const float fi = (float)i / 16;
            const float fs = sin(fi);
            return fix16_t(fs);
        }

        static consteval auto build_sintable() {
            using namespace numbers;
            array_s<fix16_t, pi2x.raw> table{};
            fix16_t* ptr = const_cast<fix16_t*>(table._data);
            for (int i = 0; i < pi2x.raw; ++i) {
                ptr[i] = generate_entry(i);
            }
            return table;
        }
        
        template<integral Int, int Bits>
        static __forceinline constexpr base_fix_t<Int, Bits> exp(base_fix_t<Int, Bits> x) {
            using fix_t = base_fix_t<Int, Bits>;
            // Check bounds: exp(-7) ≈ 0.0009, exp(7) ≈ 1096
            if (x.raw < -(static_cast<Int>(7) << Bits)) return fix_t(0);
            if (x.raw > (static_cast<Int>(7) << Bits)) {
                // Return max positive value (all bits set except sign bit)
                constexpr Int max_val = static_cast<Int>((~Int(0)) >> 1);
                return fix_t(max_val, true);
            }
            // Taylor series: exp(x) = 1 + x + x²/2! + x³/3! + x⁴/4! + ...
            fix_t result = fix_t(1);
            fix_t term = fix_t(1);
            // Compute terms until they become negligible
            for (int i = 1; i <= 12; ++i) {
                term = term.mul(x).div(i);
                result += term;
                // Stop if term becomes too small
                if (term.raw == 0) break;
            }
            return result;
        }

        template<integral Int, int Bits>
        static __forceinline constexpr base_fix_t<Int, Bits> log(base_fix_t<Int, Bits> x) {
            using fix_t = base_fix_t<Int, Bits>;
            // log(2) ≈ 0.693147180559945309417
            static constexpr const fix_t log2(0.693147180559945309417f);
            // Undefined for x <= 0, return minimum value
            if (x.raw <= 0) {
                constexpr Int min_val = static_cast<Int>(Int(1) << (sizeof(Int) * 8 - 1));
                return fix_t(min_val, true);
            }
            // Range reduction: log(x) = log(m * 2^e) = log(m) + e*log(2)
            // Normalize x to [1, 2) range
            Int raw = x.raw;
            int exponent = 0;
            // Scale to [1, 2) range by counting shifts
            while (raw >= (static_cast<Int>(2) << Bits)) {
                raw >>= 1;
                exponent++;
            }
            while (raw < (static_cast<Int>(1) << Bits)) {
                raw <<= 1;
                exponent--;
            }
            const fix_t m(raw, true);
            static constexpr fix_t one(1);
            // Use Taylor series for log(m) where m is in [1, 2)
            // log(m) = log(1 + u) where u = m - 1, u in [0, 1)
            const fix_t u = m - one;
            // log(1 + u) ≈ u - u²/2 + u³/3 - u⁴/4 + u⁵/5 - u⁶/6
            const fix_t u2 = u.mul(u);
            const fix_t u3 = u2.mul(u);
            const fix_t u4 = u3.mul(u);
            const fix_t u5 = u4.mul(u);
            const fix_t u6 = u5.mul(u);
            const fix_t log_m = u - u2.div(2) + u3.div(3) - u4.div(4) + u5.div(5) - u6.div(6);
            return log_m + fix_t(exponent).mul(log2);
        }
    }
    
    extern "C" {
        float sinf(float);
    }
    
    fix16_t sin(fix16_t x) {
        using namespace numbers;
        // Generate lookup table at compile time
        static constexpr auto s_table = detail::build_sintable();
        static constexpr int32_t upimod_max = (INT16_MAX / pi2x.raw) * pi2x.raw;
        const int32_t uraw = x < 0 ? (upimod_max + x.raw) : x.raw;
        const auto idx = div_fast(uraw, pi2x.raw).rem;
        return s_table[idx];
    }
    
    fix16_t cos(fix16_t x) {
        using namespace numbers;
        // cos(x) = sin(x + π/2)
        return sin(x + pi_2);
    }

    fix16_t tan(fix16_t x) {
        const fix16_t s = sin(x);
        const fix16_t c = cos(x);
        // Avoid division by zero
        if (c.raw == 0) {
            return fix16_t(s.raw < 0 ? INT16_MIN : INT16_MAX, true);
        } else {
            return s.div(c);
        }
    }

    fix16_t exp(fix16_t x) {
        return detail::exp(x);
    }

    fix16_t log(fix16_t x) {
        return detail::log(x);
    }

    fix16_t pow(fix16_t base, fix16_t exp) {
        using namespace numbers;
        if (base.raw <= 0) return fix16_t(0);
        if (exp.raw == 0) return numbers::one;
        if (exp.raw == one.raw) return base;  // exp == 1
        const int16_t exp_int = exp.raw >> 4;
        if ((exp_int << 4) == exp.raw) {
            // Integer exponent - use exponentiation by squaring
            fix16_t result = numbers::one;
            fix16_t b = base;
            int16_t e = exp_int;
            const bool negative_exp = e < 0;
            if (negative_exp) e = -e;
            while (e > 0) {
                if (e & 1) result = result.mul(b);
                b = b.mul(b);
                e >>= 1;
            }
            return negative_exp ? (numbers::one.div(result)) : result;
        } else {
            // General case: pow(a, b) = exp(b * log(a))
            // Use fix32_t for intermediate calculation to preserve precision
            using fix32_t = base_fix_t<int32_t, 20>;
            const fix32_t base_32 = fix32_t(static_cast<int32_t>(base.raw) << 16, true);
            const fix32_t exp_32 = fix32_t(static_cast<int32_t>(exp.raw) << 16, true);
            const fix32_t result = detail::exp(exp_32 * detail::log(base_32));
            // Add 1 << 15 to round to nearest instead of truncating
            return fix16_t(static_cast<int16_t>((result.raw + (static_cast<int32_t>(1) << 15)) >> 16), true);
        }
    }

}
