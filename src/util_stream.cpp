//
//  util_stream.cpp
//  toybox
//
//  Created by Fredrik on 2024-05-19.
//

#include "util_stream.hpp"

using namespace toybox;

ptrdiff_t substream_c::tell() const {
    auto t = _stream->tell() - _origin;
    if (t < -1 || t > _length) {
        return -1;
    }
    return t;
};

ptrdiff_t substream_c::seek(ptrdiff_t pos, seekdir_e way) {
    switch (way) {
        case seekdir_e::beg:
            _stream->seek(pos + _origin, way);
            break;
        case seekdir_e::cur:
            _stream->seek(pos, way);
            break;
        case seekdir_e::end:
            _stream->seek(pos + _origin + _length, way);
            break;
    }
    return tell();
}

size_t substream_c::read(uint8_t *buf, size_t count) {
    assert(tell() >= 0 && "Substream out of range");
    count = MIN(count, _length - tell());
    if (count > 0) {
        return _stream->read(buf, count);
    }
    return count;
}

size_t substream_c::write(const uint8_t *buf, size_t count) {
    assert(tell() >= 0 && "Substream out of range");
    count = MIN(count, _length - tell());
    if (count > 0) {
        return _stream->write(buf, count);
    }
    return count;
}
