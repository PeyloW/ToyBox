//
//  audio.cpp
//  toybox
//
//  Created by Fredrik on 2024-02-18.
//

#include "media/audio.hpp"

#if TOYBOX_TARGET_ATARI

using namespace toybox;


ymmusic_c::ymmusic_c(const char *path) {
    fstream_c file(path);
    hard_assert(file.good());
    file.seek(0, stream_c::seekdir_e::end);
    size_t size = file.tell();
    file.seek(0, stream_c::seekdir_e::beg);
    
    _sndh.reset((uint8_t *)_malloc(size));
    _length = size;
    size_t read = file.read(_sndh.get(), size);
    assert(read == size && "Failed to read complete SNDH file");
    assert(memcmp(_sndh + 12, "SNDH", 4) == 0 && "File must be a valid SNDH file");
    _title = nullptr;
    _composer = nullptr;
    _track_count = 1;
    _freq = 50;
    char *header_str = (char *)(_sndh + 16);
    while (strncmp(header_str, "HDNS", 4) != 0 && ((uint8_t*)header_str < _sndh + 200)) {
        int len = (int)strlen(header_str);
         if (len > 0) {
            if (len > 100) {
                break;
            }
            if (strncmp(header_str, "TITL", 4) == 0) {
                _title = header_str + 4;
            } else if (strncmp(header_str, "COMM", 4) == 0) {
                _composer = header_str + 4;
            } else if (strncmp(header_str, "##", 2) == 0) {
                _track_count = atoi(header_str + 2);
            } else if (strncmp(header_str, "TA", 2) == 0 ||
                       strncmp(header_str, "TB", 2) == 0 ||
                       strncmp(header_str, "TC", 2) == 0 ||
                       strncmp(header_str, "TD", 2) == 0 ||
                       strncmp(header_str, "!V", 2) == 0) {
                _freq = atoi(header_str + 2);
                assert(_freq != 0 && "Music frequency must be non-zero");
            }
        }
        header_str += len;
        while (*++header_str == 0);
    }
#ifdef __M68000__
    codegen_s::make_trampoline(_music_init_code, _sndh + 0, false);
    codegen_s::make_trampoline(_music_exit_code, _sndh + 4, false);
    codegen_s::make_trampoline(_music_play_code, _sndh + 8, false);
#endif
}

#endif
