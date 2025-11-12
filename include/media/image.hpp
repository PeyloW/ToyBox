//
//  image.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-11.
//

#pragma once

#include "media/palette.hpp"
#include "core/memory.hpp"
#include "runtime/assets.hpp"

namespace toybox {
    
    class canvas_c;
    
    /**
     An `image_c` is a read only representation of any graphics.
     An image with a mask is a sprite.
     Images can be loaded from EA 85 ILBM files, or created at runtime.
     On emulated host machines only, images can also be saved.
     NOTO: For Amiga we will need to support bitplane layout other than interweaved.
     */
    class image_c final : public asset_c {
        friend class canvas_c;
        friend class machine_c;
    public:
        enum class compression_type_e : uint8_t {
            none,
            packbits,
            vertical,  // Not supported
            deflate    // Non-standard, not supported as of now
        };
        static constexpr int MASKED_CIDX = -1;
        static __forceinline constexpr bool is_masked(int i) __pure { return i < 0; }
        enum class bitplane_layout_e : uint8_t {
            interweaved, interleaved, continious
        };
        
        image_c(const size_s size, bool masked, shared_ptr_c<palette_c> palette);
        image_c(const char *path, int masked_cidx = MASKED_CIDX);
        virtual ~image_c() {};

        __forceinline type_e asset_type() const override { return image; }

#if TOYBOX_IMAGE_SUPPORTS_SAVE
        bool save(const char *path, compression_type_e compression, bool masked, int masked_cidx = MASKED_CIDX);
#endif
                
        __forceinline void set_palette(const shared_ptr_c<palette_c> &palette) {
            _palette = palette;
        }
        __forceinline shared_ptr_c<palette_c> &palette() const {
            return *(shared_ptr_c<palette_c>*)&_palette;
        }
        __forceinline size_s size() const { return _size; }
        __forceinline bool masked() const { return _maskmap != nullptr; }
        __forceinline bitplane_layout_e layout() const { return bitplane_layout_e::interweaved; }

        int get_pixel(point_s at) const;
        void put_pixel(int ci, point_s) const;
        
    private:
        shared_ptr_c<palette_c> _palette;
        unique_ptr_c<uint16_t> _bitmap;
        uint16_t *_maskmap;
        size_s _size;
        uint16_t _line_words;
    };
    
}
