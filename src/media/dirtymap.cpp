//
//  graphics_dirtymap.cpp
//  toybox
//
//  Created by Fredrik on 2024-03-18.
//

#include "media/dirtymap.hpp"
#include "media/canvas.hpp"

using namespace toybox;

static constexpr int16_t LOOKUP_SIZE = 256;
struct  bitrun_list_s {
    int16_t num_runs;
    struct bitrun_s {
        int16_t start;
        int16_t length;
    } bit_runs[4];
};
static_assert(sizeof(bitrun_list_s) == 18, "bitrun_list_s size mismatch");
static bitrun_list_s* lookup_table[LOOKUP_SIZE];
static void init_lookup_table_if_needed() {
    static bitrun_list_s _lookup_buffer[LOOKUP_SIZE];
    static bool is_initialized = false;
    if (is_initialized) {
        return;
    }
    is_initialized = true;
    for (int16_t input = 0; input < LOOKUP_SIZE; input++) {
        bitrun_list_s* result = &_lookup_buffer[input];
        lookup_table[input] = result;
        result->num_runs = 0;
        // Iterate through each bit in the input
        for (int16_t i = 0; i < 8; ) {
            // If the current bit is 1, start a new run
            if ((input >> i) & 1) {
                int16_t start = i * TOYBOX_DIRTYMAP_TILE_SIZE.width;
                int16_t length = TOYBOX_DIRTYMAP_TILE_SIZE.width;
                result->bit_runs[result->num_runs].start = start;
                while ((input >> (i + 1)) & 1) {
                    length += TOYBOX_DIRTYMAP_TILE_SIZE.width;
                    i++;
                }
                result->bit_runs[result->num_runs].length = length;
                result->num_runs++;
            }
            i++;
        }
    }
}

dirtymap_c* dirtymap_c::create(size_s size) {
    int bytes = dirtymap_c::instance_size(&size);
    return new (_calloc(1, bytes)) dirtymap_c(size);
}

int dirtymap_c::instance_size(size_s *size) {
    init_lookup_table_if_needed();
    size->width = (size->width + (8 * TOYBOX_DIRTYMAP_TILE_SIZE.width - 1)) / (8 * TOYBOX_DIRTYMAP_TILE_SIZE.width);
    size->height /= TOYBOX_DIRTYMAP_TILE_SIZE.height;
    const int16_t data_size = size->width * (size->height + 1) + 3;
    return sizeof(dirtymap_c) + data_size;
}

void dirtymap_c::mark(const rect_s &rect) {
    const int16_t x1 = rect.origin.x / TOYBOX_DIRTYMAP_TILE_SIZE.width;
    const int16_t x2 = (rect.max_x()) / TOYBOX_DIRTYMAP_TILE_SIZE.width;
    const int16_t y1 = rect.origin.y / TOYBOX_DIRTYMAP_TILE_SIZE.height;
    assert(y1 < _size.height && "Y coordinate must be within dirtymap height");
#define BITS_PER_BYTE 8
    static constexpr uint8_t first_byte_masks[BITS_PER_BYTE] = {
        0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80
    };
    static constexpr uint8_t last_byte_masks[BITS_PER_BYTE] = {
        0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
    };

    const int16_t extra_rows = ((rect.origin.y + rect.size.height - 1) / TOYBOX_DIRTYMAP_TILE_SIZE.height - y1);
    assert(y1 + extra_rows < _size.height && "Y extent must be within dirtymap height");
    const int16_t start_byte = x1 / BITS_PER_BYTE;
    assert(start_byte < _size.width && "Start byte must be within dirtymap width");
    const int16_t end_byte = x2 / BITS_PER_BYTE;
    assert(end_byte < _size.width && "End byte must be within dirtymap width");
    const int16_t start_bit = x1 % BITS_PER_BYTE;
    assert(start_bit < 8 && "Start bit must be less than 8");
    const int16_t end_bit = x2 % BITS_PER_BYTE;
    assert(end_bit < 8 && "End bit must be less than 8");
    
    uint8_t *data = _data + (start_byte + _size.width * y1);

    if (extra_rows == 0) {
        if (start_byte == end_byte) {
            *data |= (first_byte_masks[start_bit] & last_byte_masks[end_bit]);
        } else {
            or_inc_to(first_byte_masks[start_bit], data);
            int j;
            while_dbra_count(j, end_byte - start_byte - 1) {
                *data++ = 0xFF;
            }
            *data |= (last_byte_masks[end_bit]);
        }
    } else {
        if (start_byte == end_byte) {
            auto mask = (first_byte_masks[start_bit] & last_byte_masks[end_bit]);
            for (int16_t y = 0; y <= extra_rows; y++) {
                *data |= mask;
                data += _size.width;
            }
        } else {
            auto mask_first = (first_byte_masks[start_bit]);
            auto mask_last = (last_byte_masks[end_bit]);
            for (int16_t y = 0; y <= extra_rows; y++) {
                auto line_data = data;
                *line_data++ |= mask_first;
                int j;
                while_dbra_count(j, end_byte - start_byte - 1) {
                    *line_data++ = 0xFF;
                }
                *line_data |= mask_last;
                data += _size.width;
            }
        }
    }
}

void dirtymap_c::merge(const dirtymap_c &dirtymap) {
    uint32_t *l_dest = (uint32_t*)_data;
    const uint32_t *l_source = (uint32_t*)dirtymap._data;
    int16_t long_count;
    while_dbra_count(long_count, (_size.width * _size.height + 3) / 4) {
        const uint32_t v = *l_source++;
        if (v) {
            *l_dest++ |= v;
        } else {
            l_dest++;
        }
    };
}

void dirtymap_c::restore(canvas_c &canvas, const image_c &clean_image) {
    auto &image = canvas.image();
    assert(image.size() == clean_image.size() && "Canvas and clean image sizes must match");
    assert(_size.width * TOYBOX_DIRTYMAP_TILE_SIZE.width * 8 >= clean_image.size().width && "Dirtymap width must cover image width");
    assert(_size.height * TOYBOX_DIRTYMAP_TILE_SIZE.height == clean_image.size().height && "Dirtymap height must match image height");
    assert((image.size().width % TOYBOX_DIRTYMAP_TILE_SIZE.width) == 0 && "Image width must be a multiple of tile width");
    assert((image.size().height % TOYBOX_DIRTYMAP_TILE_SIZE.height) == 0 && "Image height must be a multiple of tile height");
    const_cast<canvas_c&>(canvas).with_clipping(false, [&] {
        canvas.with_dirtymap(nullptr, [&] {
            auto draw = [&](const rect_s& rect) {
                canvas.draw_aligned(clean_image, rect, rect.origin);
            };
            restore_f func(draw);
            restore(func);
        });
    });
}

void dirtymap_c::restore(function_c<void(const rect_s&)>& func) {
#if TOYBOX_DEBUG_DIRTYMAP
    static uint32_t s_restore_generation = 0;
    s_restore_generation++;
#endif
    auto data = _data;
    point_s at = {0, 0};
    int row;
    while_dbra_count(row, _size.height) {
        int col;
        while_dbra_count(col, _size.width) {
            const uint8_t byte = *data;
            if (byte) {
                const int16_t height = [&] {
                    int16_t height = 0;
                    do {
                        data[height * _size.width] = 0;
                        height++;
                    } while (data[height * _size.width] == byte);
                    return height * TOYBOX_DIRTYMAP_TILE_SIZE.height;
                }();
                auto bitrunlist = lookup_table[(int16_t)byte];
                int16_t *bitrun = (int16_t*)bitrunlist->bit_runs;
                int r;
                while_dbra_count(r, bitrunlist->num_runs) {
                    rect_s rect;
                    rect.origin = point_s(at.x + *bitrun++, at.y);
                    rect.size = size_s(*bitrun++, height);
#if TOYBOX_DEBUG_DIRTYMAP
                    printf("Restore [%u] {{%d, %d}, {%d, %d}}\n", s_restore_generation, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
#endif
                    func(rect);
                }
            }
            data++;
            at.x += TOYBOX_DIRTYMAP_TILE_SIZE.width * 8;
        }
        at.x = 0;
        at.y += TOYBOX_DIRTYMAP_TILE_SIZE.height;
    }
}

void dirtymap_c::clear() {
    memset(_data, 0, _size.width * _size.height);
}

#if TOYBOX_DEBUG_DIRTYMAP
void dirtymap_c::debug(const char *name) const {
    printf("Dirtymap %d columns [%s]\n", _size.width, name);
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    ((byte) & 0x01 ? '1' : '0'), \
    ((byte) & 0x02 ? '1' : '0'), \
    ((byte) & 0x04 ? '1' : '0'), \
    ((byte) & 0x08 ? '1' : '0'), \
    ((byte) & 0x10 ? '1' : '0'), \
    ((byte) & 0x20 ? '1' : '0'), \
    ((byte) & 0x40 ? '1' : '0'), \
    ((byte) & 0x80 ? '1' : '0')
    auto data = _data;
    int row;
    while_dbra_count(row, _size.height) {
        int col;
        while_dbra_count(col, _size.width) {
            const auto byte = *data++;
            printf(BYTE_TO_BINARY_PATTERN,  BYTE_TO_BINARY(byte));
        }
        printf("\n");
    }
}
#endif
