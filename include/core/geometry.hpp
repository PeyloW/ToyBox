//
//  geometry.hpp
//  toybox
//
//  Created by Fredrik on 2024-02-11.
//

#pragma once

#include "core/cincludes.hpp"
#include "core/utility.hpp"
#include "core/math.hpp"

namespace toybox {

#pragma mark - Base geometric types
    
    template<typename Type>
    struct base_point_s {
        constexpr base_point_s() : x(0), y(0) {}
        constexpr base_point_s(int16_t x, int16_t y) : x(x), y(y) {}
        Type x, y;
        constexpr bool operator==(const base_point_s& p) const {
            return x == p.x && y == p.y;
        }
        template<typename OType>
        constexpr explicit operator base_point_s<OType>() const {
            return base_point_s<OType>(static_cast<OType>(x), static_cast<OType>(y));
        }
        constexpr base_point_s operator+(base_point_s o) const {
            return base_point_s(x + o.x, y + o.y);
        }
        constexpr base_point_s operator-(base_point_s o) const {
            return base_point_s(x - o.x, y - o.y);
        }
    };
    
    template<typename Type>
    struct base_size_s {
        using point_s = base_point_s<Type>;
        constexpr base_size_s() : width(0), height(0) {}
        constexpr base_size_s(int16_t w, int16_t h) : width(w), height(h) {}
        Type width, height;
        constexpr bool operator==(const base_size_s s) const {
            return width == s.width && height == s.height;
        }
        template<typename OType>
        constexpr explicit operator base_size_s<OType>() const {
            return base_size_s<OType>(static_cast<OType>(width), static_cast<OType>(height));
        }
        constexpr bool contains(const point_s point) const {
            return point.x >= 0 && point.y >= 0 && point.x < width && point.y < height;
        }
        constexpr bool contained_by(const base_size_s size) const {
            return width <= size.width && height <= size.height;
        }
        constexpr bool is_empty() const {
            return width <= 0 || height <= 0;
        }
    };

    template<typename Type>
    struct base_rect_s {
        using point_s = base_point_s<Type>;
        using size_s = base_size_s<Type>;
        constexpr base_rect_s() : origin(), size() {}
        constexpr base_rect_s(const size_s& s) : origin(), size(s) {}
        constexpr base_rect_s(const point_s& o, const size_s& s) : origin(o), size(s) {}
        constexpr base_rect_s(int16_t x, int16_t y, int16_t w, int16_t h) : origin(x, y), size(w, h) {}

        point_s origin;
        size_s size;
        __forceinline constexpr int16_t max_x() const { return origin.x + size.width - 1; }
        __forceinline constexpr int16_t max_y() const { return origin.y + size.height - 1; }
        constexpr bool operator==(const Type& r) const {
            return origin == r.origin && size == r.size;
        }
        template<typename OType>
        constexpr explicit operator base_rect_s<OType>() const {
            return base_rect_s<OType>(static_cast<base_point_s<OType>>(origin), static_cast<base_size_s<OType>>(size));
        }
        constexpr base_rect_s operator+(base_point_s<Type> o) const {
            return base_rect_s(origin + o, size);
        }
        constexpr base_rect_s operator-(base_point_s<Type> o) const {
            return base_rect_s(origin - o, size);
        }
        constexpr bool contains(const point_s point) const {
            const point_s at = point_s(point.x - origin.x, point.y - origin.y);
            return size.contains(at);
        }
        constexpr bool contained_by(const base_rect_s& rect) const {
            if (origin.x < rect.origin.x || origin.y < rect.origin.y) return false;
            if (max_x() > rect.max_x()) return false;
            if (max_y() > rect.max_y()) return false;
            return true;
        }
        /// Clip this rect to the bounds of clip_bounds, adjusting at accordingly.
        /// Returns true if the rect was completely clipped (size became <= 0).
        bool clip_to(const base_rect_s& clip_bounds, point_s& at) {
            bool did_clip = false;

            // Clip left edge
            if (at.x < clip_bounds.origin.x) {
                const auto delta = clip_bounds.origin.x - at.x;
                this->size.width -= delta;
                if (this->size.width <= 0) return true;
                this->origin.x += delta;
                at.x = clip_bounds.origin.x;
                did_clip = true;
            }

            // Clip top edge
            if (at.y < clip_bounds.origin.y) {
                const auto delta = clip_bounds.origin.y - at.y;
                this->size.height -= delta;
                if (this->size.height <= 0) return true;
                this->origin.y += delta;
                at.y = clip_bounds.origin.y;
                did_clip = true;
            }

            // Clip right edge
            const auto right_overshoot = (at.x + this->size.width) - (clip_bounds.origin.x + clip_bounds.size.width);
            if (right_overshoot > 0) {
                this->size.width -= right_overshoot;
                if (this->size.width <= 0) return true;
                did_clip = true;
            }

            // Clip bottom edge
            const auto bottom_overshoot = (at.y + this->size.height) - (clip_bounds.origin.y + clip_bounds.size.height);
            if (bottom_overshoot > 0) {
                this->size.height -= bottom_overshoot;
                if (this->size.height <= 0) return true;
                did_clip = true;
            }

            return did_clip;
        }
    };

#pragma mark - Integral geometry
    
    using point_s = base_point_s<int16_t>;
    static_assert(sizeof(point_s) == 4);

    using size_s = base_size_s<int16_t>;
    static_assert(sizeof(point_s) == 4);

    using rect_s = base_rect_s<int16_t>;
    static_assert(sizeof(rect_s) == 8);

#pragma mark - Real geometry

    using fpoint_s = base_point_s<fix16_t>;
    static_assert(sizeof(fpoint_s) == 4);

    using fsize_s = base_size_s<fix16_t>;
    static_assert(sizeof(fpoint_s) == 4);

    using frect_s = base_rect_s<fix16_t>;
    static_assert(sizeof(frect_s) == 8);

    struct fcrect_s {
        fpoint_s center;
        fsize_s size;
    };
    
}

