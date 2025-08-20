#pragma once

#include <string>

namespace Courage::Network {

class Client {
public:
    Client(int socketFd, const std::string& username);
    ~Client();

    std::string getOfflineUUID() const { return offlineUUID_; }
    std::string getOnlineUUID() const { return onlineUUID_; }

    int getSocket() const { return socketFd_; }

    void setOnlineUUID(const std::string& uuid);

private:
    int socketFd_;
    std::string username_;

    std::string offlineUUID_;
    std::string onlineUUID_;

    void generateOfflineUUID(const std::string& username);
};

}
