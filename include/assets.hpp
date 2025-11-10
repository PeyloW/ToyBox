//
//  assets.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-26.
//

#pragma once

#include "stream.hpp"
#include "vector.hpp"
#include "memory.hpp"
#include "concepts.hpp"


namespace toybox {

    using namespace toybox;
    
    class asset_c : nocopy_c {
    public:
        enum class type_e : uint8_t {
            custom, image, tileset, tilemap, font, sound, music
        };
        using enum type_e;
        __forceinline virtual type_e asset_type() const __pure { return type_e::custom; }
    };
    
    class image_c;
    class tileset_c;
    class font_c;
    class sound_c;
    class music_c;

    /**
     `asset_manager_c` handles loading and unloading of assets from disk.
     The asset manager is a singleton, intended for subclassing for each client
     of toybox.
     The client is expected to set the singleton.
     */
    class asset_manager_c final : nocopy_c {
    public:
        static asset_manager_c &shared();
        
        ~asset_manager_c() {}

        using progress_f = void(*)(int loaded, int total);
        void preload(uint32_t sets, progress_f progress = nullptr);
        void unload(uint32_t sets);
        
        asset_c &asset(int id) const;

        template<derived_from<asset_c> T>
        __forceinline T &asset(int id) const { return (T&)(asset(id)); };

        __forceinline image_c &image(int id) const { return (image_c&)(asset(id)); }
        __forceinline tileset_c &tileset(int id) const { return (tileset_c&)(asset(id)); }
        __forceinline font_c &font(int id) const { return (font_c&)(asset(id)); }
        __forceinline sound_c &sound(int id) const { return (sound_c&)(asset(id)); }
        __forceinline music_c &music(int id) const { return (music_c&)(asset(id)); }

        unique_ptr_c<char> data_path(const char* file) const;
        unique_ptr_c<char> user_path(const char* file) const;

        struct asset_def_s {
            using asset_create_f = asset_c*(*)(const asset_manager_c &manager, const char* path);
            constexpr asset_def_s(asset_c::type_e type, uint32_t sets, const char* file = nullptr, asset_create_f create = nullptr) :
                type(type), sets(sets), file(file), create(create) {}
            asset_c::type_e type;
            uint32_t sets;
            const char* file;
            asset_create_f create;
        };

        void add_asset_def(int id, const asset_def_s &def);
        int add_asset_def(const asset_def_s &def);

    private:
        asset_manager_c();
        asset_c* create_asset(int id, const asset_def_s &def) const;

        vector_c<asset_def_s, TOYBOX_ASSET_COUNT> _asset_defs;
        mutable vector_c<unique_ptr_c<asset_c>, TOYBOX_ASSET_COUNT> _assets;
    };
    
}
