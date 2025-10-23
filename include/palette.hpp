//
//  graphics.hpp
//  toybox
//
//  Created by Fredrik on 2024-02-04.
//

#pragma once

#include "cincludes.hpp"
#include "types.hpp"
#include "algorithm.hpp"
#include "display_list.hpp"

namespace toybox {
    
    using namespace toybox;

    /**
     A 12 bit color in a 16 bit word.
     TODO: Make STe hacks for Atari target only.
     */
    class color_c {
    public:
        uint16_t color;
        constexpr color_c() = default;
        constexpr color_c(uint16_t c) : color(c) {}
        constexpr color_c(const uint8_t r, const uint8_t g, const uint8_t b) : color(to_ste(r, 8) | to_ste(g, 4) | to_ste(b, 0)) {}
        void set_at(const int i) const {
#ifdef __M68000__
#   if TOYBOX_TARGET_ATARI
            reinterpret_cast<uint16_t*>(0xffff8240)[i] = color;
#   else
#       error "Unsupported target"
#   endif
#endif
        }
        void get(uint8_t *r, uint8_t *g, uint8_t *b) const {
            *r = from_ste(color, 8);
            *g = from_ste(color, 4);
            *b = from_ste(color, 0);
        }
        color_c mix(color_c other, int shade) const;
        static constexpr int MIX_FULLY_THIS = 0;
        static constexpr int MIX_FULLY_OTHER = 64;
    private:
        __forceinline static constexpr uint16_t to_ste(const uint8_t c, const uint8_t shift) {
            constexpr uint8_t STE_TO_SEQ[16] = { 0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15 };
            return STE_TO_SEQ[c >> 4] << shift;
        }
        __forceinline static constexpr uint8_t from_ste(const uint16_t c, const uint8_t shift) {
            constexpr uint8_t STE_FROM_SEQ[16] = { 0x00, 0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee,
                0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff};
            return STE_FROM_SEQ[(c >> shift) & 0x0f];
        }
    };
        
    /**
     A `basic_palette_c` is an arbitaruy list of colors.
     */
    template<int Count>
    class basic_palette_c : public nocopy_c {
    public:
        using value_type = color_c;
        using pointer = value_type* ;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = value_type*;
        using const_iterator = const value_type*;

        basic_palette_c() : _colors(0) { }
        basic_palette_c(uint16_t *cs) { memcpy(_colors, cs, sizeof(_colors)); }
        basic_palette_c(uint8_t *c) {
            c += 3 * Count;
            int i;
            do_dbra(i, Count - 1) {
                c -= 3;
                _colors[i] = color_c(c[0], c[1], c[2]);
            } while_dbra(i);
        }
        const uint16_t *ptr() const __pure { return &_colors[0].color; }

        __forceinline iterator begin() __pure { return &_colors[0]; }
        __forceinline const_iterator begin() const __pure { return &_colors[0]; }
        __forceinline iterator end() __pure { return &_colors[Count]; }
        __forceinline const_iterator end() const __pure { return &_colors[Count]; }
        __forceinline int size() const __pure { return Count; }
        
        inline reference operator[](const int i) __pure {
            assert( i >= 0 && i < Count);
            return _colors[i];
        }
        inline const_reference operator[](const int i) const __pure {
            assert( i >= 0 && i < Count);
            return _colors[i];
        }

    private:
        color_c _colors[Count];
    };
        
    /**
     A `palette_c` is a specialized list of colors with exactly 16 colors.
     TODO: Should Amiga target allow for 32?
     */
    class palette_c : public basic_palette_c<16>, public display_item_c {
    public:
        type_e display_type() const override { return palette; }
        palette_c() : basic_palette_c<16>() {}
        palette_c(uint16_t *cs) : basic_palette_c<16>(cs) {}
        palette_c(uint8_t *c) : basic_palette_c<16>(c) {}
    };
    
}
