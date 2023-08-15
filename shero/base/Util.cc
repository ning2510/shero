#include "shero/base/Util.h"
#include "shero/base/Macro.h"

#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <iostream>

namespace shero {

static thread_local pid_t tid = 0;

pid_t GetPid() {
    return getpid();
}

pid_t GetThreadId() {
    if(SHERO_UNLICKLY(tid == 0)) {
        tid = syscall(SYS_gettid);
    }
    return tid;
}

// 毫秒 ms
uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

// 微秒 µs
uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}

std::string Date2Str(time_t ts /*= time(0)*/, const std::string &format /*= "%Y%m%d"*/) {
    return Time2Str(ts, format);
}

std::string Time2Str(time_t ts /*= time(0)*/, const std::string &format /*= "%Y-%m-%d %H:%M:%S"*/) {
    struct tm tm;
    bzero(&tm, sizeof(tm));
    localtime_r(&ts, &tm);
    char buf[64] = {0};
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
}

std::string SHA1sum(const std::string &data) {
    return SHA1sum(data.c_str(), data.size());
}

std::string SHA1sum(const void *data, size_t len) {
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, data, len);
    std::string result;
    result.resize(SHA_DIGEST_LENGTH);
    SHA1_Final((unsigned char*)&result[0], &ctx);
    return result;
}

std::string Encodebase64(const std::string &data) {
    return Encodebase64(data.c_str(), data.size());
}

std::string Encodebase64(const void *data, size_t len) {
    BIO *b64, *bio;
    BUF_MEM *bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, data, len);
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bptr);
    std::string result(bptr->data, bptr->length - 1);

    BIO_free_all(bio);

    return result;
}

std::string RandomString(size_t len) {
    if(len == 0) {
        return "";
    }
    static const char CHARS[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::string rt;
    rt.resize(len);
    for(size_t i = 0; i < len; ++i) {
        rt[i] = CHARS[rand() % sizeof(CHARS)];
    }
    return rt;
}

}   // namespace shero