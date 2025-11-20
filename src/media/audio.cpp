//
//  audio.cpp
//  toybox
//
//  Created by Fredrik on 2024-02-18.
//

#include "media/audio.hpp"
#include "core/iffstream.hpp"

using namespace toybox;

namespace cc4 {
    static constexpr cc4_t AIFF("AIFF");
    static constexpr cc4_t COMM("COMM");
    static constexpr cc4_t SSND("SSND");
}

struct  extended80_s {
    uint16_t exp;
    uint16_t fracs[4];
    uint16_t to_uint16() const {
        int16_t exponent = (exp & 0x7fff) - 16383;
        uint16_t significant = 0;
        significant = fracs[0];
        if (exponent > 15) {
            return 0;
        } else {
            return significant >> ( 15 - exponent );
        }
    }
};
static_assert(sizeof(extended80_s) == 10, "extended80_t size mismatch");

struct __attribute__((packed)) aiff_common_s {
    int16_t num_channels;
    uint32_t num_sample_frames;
    int16_t sample_size;
    extended80_s sample_rate;
};
static_assert(sizeof(aiff_common_s) == 18, "aiff_common_t size mismatch");
namespace toybox {
    template<>
    struct struct_layout<aiff_common_s> {
        static constexpr const char* value = "1w1l6w";
    };
}

struct  aiff_ssnd_data_s {
    uint32_t offset;
    uint32_t block_size;
    uint8_t data[];
};
static_assert(sizeof(aiff_ssnd_data_s) == 8, "ssnd_data_t size mismatch");
namespace toybox {
    template<>
    struct struct_layout<aiff_ssnd_data_s> {
        static constexpr const char* value = "2l";
    };
}

sound_c::sound_c(const char* path) :
    _sample(nullptr),
    _length(0),
    _rate(0)
{
    iffstream_c file(path);
    iff_group_s form;
    if (!file.good() || !file.first(cc4::FORM, ::cc4::AIFF, form)) {
        hard_assert(0 && "Failed to load AIFF file");
        return; // Not a AIFF
    }
    iff_chunk_s chunk;
    aiff_common_s common;
    while (file.next(form, cc4::ANY, chunk)) {
        if (chunk.id == ::cc4::COMM) {
            if (!file.read(&common)) {
                return;
            }
            assert(common.num_channels == 1 && "Only mono audio is supported");
            assert(common.sample_size == 8 && "Only 8-bit audio is supported");
            _length = common.num_sample_frames;
            _rate = common.sample_rate.to_uint16();
            assert(_rate >= 11000 && _rate <= 14000 && "Sample rate must be between 11kHz and 14kHz");
        } else if (chunk.id == ::cc4::SSND) {
            aiff_ssnd_data_s data;
            if (!file.read(&data)) {
                return;
            }
            assert(data.offset == 0 && "SSND offset must be zero");
            assert(chunk.size - 8 == common.num_sample_frames && "SSND data size must match sample frame count");
            _sample.reset((int8_t*)_malloc(_length));
            file.read(_sample.get(), _length);
        } else {
#ifndef __M68000__
            printf("Skipping '%s'\n", chunk.id.cstring());
#endif
            file.skip(chunk);
        }
    }
}
