#include "Courage/Network/Client.hpp"
#include <mbedtls/md5.h>
#include <uuid/uuid.h>
#include <unistd.h>
#include <cstring>

namespace Courage::Network {

Client::Client(int socketFd, const std::string& username)
    : socketFd_(socketFd), username_(username) {
    generateOfflineUUID(username_);
}

Client::~Client() {
    if (socketFd_ > 0) {
        ::close(socketFd_);
    }
}

void Client::generateOfflineUUID(const std::string& username) {
    std::string base = "OfflinePlayer:" + username;

    unsigned char md5sum[16];

    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);
    mbedtls_md5_update_ret(&ctx, reinterpret_cast<const unsigned char*>(base.data()), base.size());
    mbedtls_md5_finish_ret(&ctx, md5sum);
    mbedtls_md5_free(&ctx);

    md5sum[6] = (md5sum[6] & 0x0F) | 0x30;
    md5sum[8] = (md5sum[8] & 0x3F) | 0x80;

    uuid_t binuuid;
    std::memcpy(binuuid, md5sum, 16);

    char uuidStr[37];
    uuid_unparse(binuuid, uuidStr);

    offlineUUID_ = uuidStr;
}

void Client::setOnlineUUID(const std::string& uuid) {
    onlineUUID_ = uuid;
}

}
