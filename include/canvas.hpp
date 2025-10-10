//
//  canvas.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-10.
//

#ifndef canvas_hpp
#define canvas_hpp

#include "image.hpp"
#include "tileset.hpp"
#include "font.hpp"
#include "dirtymap.hpp"
#include "concepts.hpp"

namespace toybox {
    
    class canvas_c : public nocopy_c {
    public:
        class remap_table_c : nocopy_c {
        public:
            constexpr remap_table_c() { for (int i = -1; i < 16; i++) _table[i + 1] = i; }
            template<int Count>
            constexpr remap_table_c(const pair_c<int, int> (&maps)[Count]) : remap_table_c() {
                for (const auto &map : maps) {
                    assert(map.first >= -1 && map.first <16);
                    assert(map.second >= -1 && map.second <16);
                    _table[map.first + 1] = map.second;
                }
            }
            int& operator[](int i) { assert(i >= -1 && i < 16); return _table[i + 1]; }
            const int& operator[](int i) const { assert(i >= -1 && i < 16); return _table[i + 1]; }
        private:
            int _table[17];
        };
        
        static const int STENCIL_FULLY_TRANSPARENT = 0;
        static const int STENCIL_FULLY_OPAQUE = 64;
        using stencil_t = uint16_t[16];
        enum class stencil_e : uint8_t {
            none,
            orderred,
            noise,
            diagonal,
            circle,
            random
        };
        static stencil_e effective_type(stencil_e type);
        
        enum class alignment_e : uint8_t {
            left,
            center,
            right
        };
        
        canvas_c(image_c &image) : _image(image) {};
        ~canvas_c() {};
        
        image_c &image() const { return _image; }
        size_s size() const { return _image.size(); }
        
        static const stencil_t *const stencil(stencil_e type, int shade);
        
        dirtymap_c *create_dirtymap() const __pure;
        
        void remap_colors(const remap_table_c &table,  const rect_s &rect) const;
        
        static void make_stencil(stencil_t stencil, stencil_e type, int shade);
        
        template<invocable<> Commands>
        __forceinline void with_clipping(bool clip, Commands commands) const {
            const bool old_clip = _clipping;
            _clipping = clip;
            commands();
            _clipping = old_clip;
        }
        
        template<invocable<> Commands>
        __forceinline void with_stencil(const stencil_t *const stencil, Commands commands) const {
            const auto old_stencil = _stencil;
            _stencil = stencil;
            commands();
            _stencil = old_stencil;
        }
        
        template<invocable<> Commands>
        __forceinline void with_dirtymap(dirtymap_c *dirtymap, Commands commands) const {
            dirtymap_c *old_dirtymap = _dirtymap;
            _dirtymap = dirtymap;
            commands();
            _dirtymap = old_dirtymap;
        }
        
        void fill(uint8_t ci, const rect_s &rect) const;
        
        void draw_aligned(const image_c &src, point_s at) const;
        void draw_aligned(const image_c &src, const rect_s &rect, point_s at) const;
        void draw_aligned(const tileset_c &src, int idx, point_s at) const;
        void draw_aligned(const tileset_c &src, point_s tile, point_s at) const;
        void draw(const image_c &src, point_s at, int color = image_c::MASKED_CIDX) const;
        void draw(const image_c &src, const rect_s &rect, point_s at, int color = image_c::MASKED_CIDX) const;
        void draw(const tileset_c &src, int idx, point_s at, int color = image_c::MASKED_CIDX) const;
        void draw(const tileset_c &src, point_s tile, point_s at, int color = image_c::MASKED_CIDX) const;
        
        void draw_3_patch(const image_c &src, int16_t cap, const rect_s &in) const;
        void draw_3_patch(const image_c &src, const rect_s &rect, int16_t cap, const rect_s &in) const;
        
        size_s draw(const font_c &font, const char *text, point_s at, alignment_e alignment = alignment_e::center, int color = image_c::MASKED_CIDX) const;
        size_s draw(const font_c &font, const char *text, const rect_s &in, uint16_t line_spacing = 0, alignment_e alignment = alignment_e::center, int color = image_c::MASKED_CIDX) const;
        
    protected:
        image_c &_image;
        mutable dirtymap_c *_dirtymap = nullptr;
        mutable const stencil_t *_stencil = nullptr;
        mutable bool _clipping = true;
        
        void imp_fill(uint8_t ci, const rect_s &rect) const;
        void imp_draw_aligned(const image_c &srcImage, const rect_s &rect, point_s point) const;
        void imp_draw(const image_c &srcImage, const rect_s &rect, point_s point) const;
        void imp_draw_masked(const image_c &srcImage, const rect_s &rect, point_s point) const;
        void imp_draw_color(const image_c &srcImage, const rect_s &rect, point_s point, uint16_t color) const;
        
        void imp_draw_rect_SLOW(const image_c &srcImage, const rect_s &rect, point_s point) const;
        
    };
    
}

#endif /* canvas_hpp */
