#pragma once

#include <string>

#include <openssl/md5.h>

template<typename T>
struct md5hash;

template<>
struct md5hash<std::string> {
    size_t operator()(const std::string& str) const {
        unsigned char result[MD5_DIGEST_LENGTH];
        MD5((unsigned char*)(str.data()), str.size(), result);
        // Maybe I mixed order here...
        return *(size_t*)(result + sizeof(result) - sizeof(size_t));
    }
};

template<>
struct md5hash<size_t> {
    size_t operator()(const size_t& val) {
        unsigned char result[MD5_DIGEST_LENGTH];
        MD5((unsigned char*)(&val), sizeof(size_t), result);
        // Maybe I mixed order here...
        return *(size_t*)(result + sizeof(result) - sizeof(size_t));
    }
};