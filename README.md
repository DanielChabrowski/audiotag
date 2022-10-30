# audiotag

C++20 audio metadata reader, very much work in progress.

Intended as research of c++20 features and audio tagging methods as well as as a replacement of taglib in https://github.com/DanielChabrowski/audio-player


### Dependencies
- CPM (https://github.com/cpm-cmake/CPM.cmake)
- utfcpp (https://github.com/nemtrif/utfcpp)
- frozen (https://github.com/serge-sans-paille/frozen)
- doctest (https://github.com/doctest/doctest)


### Why not taglib?

* still uses c++03
* tries to preserve API and ABI compatibility
* does a lot of heap memory allocations, even for things like comparing tag identifiers or converting bytes to integers - and since ByteVector is ref counted we'll get at least 2 allocations for each
* does a lot of fseeks, maybe it's justified, I don't know yet
* decodes tag frames at the time of file parsing (as of now, I try to decode tag content lazily, we'll see how that works out)
