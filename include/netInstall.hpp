#pragma once

#include <install/install.hpp>

#include <unordered_map>
#include <string>
#include <functional>

using namespace std;

namespace net {
    Result InitializeServerSocket();
    void CloseServerSocket();
    vector<string> GetUrls();
    shared_ptr<tin::install::Install> GetNetTask(string url, NcmStorageId destStorage);
    void SendAck();
}