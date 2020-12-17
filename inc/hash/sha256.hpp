#pragma once

#include <string>

#include <openssl/sha.h>

template<typename T>
struct sha256hash;

template<>
struct sha256hash<std::string> {
    size_t operator()(const std::string& str) const {
        unsigned char result[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)(str.data()), str.size(), result);
        // Maybe I mixed order here...
        return *(size_t*)(result + sizeof(result) - sizeof(size_t));
    }
};

template<>
struct sha256hash<size_t> {
    size_t operator()(const size_t& val) {
        unsigned char result[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)(&val), sizeof(size_t), result);
        // Maybe I mixed order here...
        return *(size_t*)(result + sizeof(result) - sizeof(size_t));
    }
};