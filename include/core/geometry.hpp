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
        constexpr bool operator==(const base_point_s &p) const {
            return x == p.x && y == p.y;
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
        constexpr bool contains(const point_s point) const {
            return point.x >= 0 && point.y >= 0 && point.x < width && point.y < height;
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
        constexpr base_rect_s(const size_s &s) : origin(), size(s) {}
        constexpr base_rect_s(const point_s &o, const size_s &s) : origin(o), size(s) {}
        constexpr base_rect_s(int16_t x, int16_t y, int16_t w, int16_t h) : origin(x, y), size(w, h) {}

        point_s origin;
        size_s size;
        __forceinline constexpr int16_t max_x() const { return origin.x + size.width - 1; }
        __forceinline constexpr int16_t max_y() const { return origin.y + size.height - 1; }
        constexpr bool operator==(const Type& r) const {
            return origin == r.origin && size == r.size;
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
        // TODO: This is non-obvious API and should be redone
        bool clip_to(const size_s size, point_s &at) {
            bool did_clip = false;
            if (at.x < 0) {
                this->size.width += at.x;
                if (this->size.width <= 0) return true;
                this->origin.x -= at.x;
                at.x = 0;
                did_clip = true;
            }
            if (at.y < 0) {
                this->size.height += at.y;
                if (this->size.height <= 0) return true;
                this->origin.y -= at.y;
                at.y = 0;
                did_clip = true;
            }
            const auto dx = size.width - (at.x + this->size.width);
            if (dx < 0) {
                this->size.width += dx;
                if (this->size.width <= 0) return true;
                did_clip = true;
            }
            const auto dy = size.height - (at.y + this->size.height);
            if (dy < 0) {
                this->size.height += dy;
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

