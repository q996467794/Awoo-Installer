/*
Copyright (c) 2017-2018 Adubbz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstring>
#include <sys/socket.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sstream>
#include <curl/curl.h>

#include <switch.h>
#include "util/network_util.hpp"
#include "install/install_nsp.hpp"
#include "install/http_nsp.hpp"
#include "install/install_xci.hpp"
#include "install/http_xci.hpp"
#include "install/install.hpp"
#include "util/error.hpp"

#include "netInstall.hpp"
#include "util/config.hpp"
#include "util/util.hpp"
#include "util/curl.hpp"

const unsigned int MAX_URL_SIZE = 1024;
const unsigned int MAX_URLS = 256;
const int REMOTE_INSTALL_PORT = 2000;
static int m_serverSocket = 0;
static int m_clientSocket = 0;

namespace net{
    Result InitializeServerSocket() try
    {
        ASSERT_OK(curl_global_init(CURL_GLOBAL_ALL), "Curl failed to initialized");

        // Create a socket
        m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

        if (m_serverSocket <= -1)
        {
            THROW_FORMAT("Failed to create a server socket. Error code: %u\n", errno);
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(REMOTE_INSTALL_PORT);
        server.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(m_serverSocket, (struct sockaddr*) &server, sizeof(server)) < 0)
        {
            THROW_FORMAT("Failed to bind server socket. Error code: %u\n", errno);
        }

        // Set as non-blocking
        fcntl(m_serverSocket, F_SETFL, fcntl(m_serverSocket, F_GETFL, 0) | O_NONBLOCK);

        if (listen(m_serverSocket, 5) < 0) 
        {
            THROW_FORMAT("Failed to listen on server socket. Error code: %u\n", errno);
        }

        if (m_serverSocket <= 0)
        {
            THROW_FORMAT("Server socket failed to initialize.\n");
        }

        return 0;
    }
    catch (std::exception& e)
    {
        LOG_DEBUG("Failed to initialize server socket!\n");
        fprintf(stdout, "%s", e.what());

        if (m_serverSocket != 0)
        {
            close(m_serverSocket);
            m_serverSocket = 0;
        }
        return -1;
    }

    void CloseServerSocket() {
        if (m_clientSocket != 0) {
            close(m_clientSocket);
            m_clientSocket = 0;
        }
        if (m_serverSocket != 0)
        {
            close(m_serverSocket);
            m_serverSocket = 0;
        }
        curl_global_cleanup();
    }

    shared_ptr<tin::install::Install> GetNetTask(string url, NcmStorageId destStorage) {
        if (inst::curl::downloadToBuffer(url, 0x100, 0x103) == "HEAD") {
            auto httpXCI = new tin::install::xci::HTTPXCI(url);
            return shared_ptr<tin::install::xci::XCIInstallTask>(new tin::install::xci::XCIInstallTask(destStorage, inst::config::ignoreReqVers, httpXCI));
        } else {
            auto httpNSP = new tin::install::nsp::HTTPNSP(url);
            return shared_ptr<tin::install::nsp::NSPInstall>(new tin::install::nsp::NSPInstall(destStorage, inst::config::ignoreReqVers, httpNSP));
        }
    }

    void SendAck() {
        LOG_DEBUG("%s\n", "Telling the server we're done installing");
        // Send 1 byte ack to close the server
        u8 ack = 0;
        tin::network::WaitSendNetworkData(m_clientSocket, &ack, sizeof(u8));
    }

    std::vector<std::string> GetUrls() try {
        std::vector<std::string> urls;

        struct sockaddr_in client;
        socklen_t clientLen = sizeof(client);

        m_clientSocket = accept(m_serverSocket, (struct sockaddr*)&client, &clientLen);

        if (m_clientSocket >= 0)
        {
            LOG_DEBUG("%s\n", "Server accepted");
            u32 size = 0;
            tin::network::WaitReceiveNetworkData(m_clientSocket, &size, sizeof(u32));
            size = ntohl(size);

            LOG_DEBUG("Received url buf size: 0x%x\n", size);

            if (size > MAX_URL_SIZE * MAX_URLS)
            {
                THROW_FORMAT("URL size %x is too large!\n", size);
            }

            // Make sure the last string is null terminated
            auto urlBuf = std::make_unique<char[]>(size+1);
            memset(urlBuf.get(), 0, size+1);

            tin::network::WaitReceiveNetworkData(m_clientSocket, urlBuf.get(), size);

            // Split the string up into individual URLs
            std::stringstream urlStream(urlBuf.get());
            std::string segment;

            while (std::getline(urlStream, segment, '\n'))
                urls.push_back(segment);
        }
        else if (errno != EAGAIN)
        {
            return {};
        }

        return urls;
    } catch (std::runtime_error& e) {
        LOG_DEBUG("Failed to perform remote install!\n");
        LOG_DEBUG("%s", e.what());
        fprintf(stdout, "%s", e.what());
        return {};
    }
}
