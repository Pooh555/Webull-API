#include "cryptography.hpp"

#include <spdlog/spdlog.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>

namespace utilities::cryptography {

std::string compute_hmac_sha1(std::string_view key, std::string_view message) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int  hash_length { 0u };

    HMAC(
        EVP_sha1(),
        key.data(),
        static_cast<int>(key.size()),
        reinterpret_cast<const unsigned char*>(message.data()),
        message.size(),
        hash,
        &hash_length
    );

    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO* bio = BIO_push(b64, mem);

    BIO_write(bio, hash, static_cast<int>(hash_length));
    BIO_flush(bio);

    BUF_MEM* buffer = nullptr;
    BIO_get_mem_ptr(bio, &buffer);

    std::string result(buffer->data, buffer->length);

    BIO_free_all(bio);

    return result;
}

std::string compute_md5(std::string_view data) {
          EVP_MD_CTX* context = EVP_MD_CTX_new();
    const EVP_MD* md      = EVP_md5();
    
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int  length { 0u };

    EVP_DigestInit_ex(context, md, nullptr);
    EVP_DigestUpdate(context, data.data(), data.size());
    EVP_DigestFinal_ex(context, hash, &length);
    EVP_MD_CTX_free(context);

    std::stringstream ss;
    
    for (uint32_t i { 0u }; i < length; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    std::string md5_str = ss.str();

    for (char& c : md5_str) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    
    return md5_str;
}

std::string generate_nonce(size_t length) {
    static constexpr std::string_view     digits = "0123456789";
    
    std::random_device                    rd;
    std::mt19937_64                       gen(rd());
    std::uniform_int_distribution<size_t> dist(0, digits.size() - 1);

    std::string nonce = "";

    nonce.reserve(length);

    for (size_t i { 0uz }; i < length; ++i) {
        nonce += digits[dist(gen)];
    }

    return nonce;
}

}