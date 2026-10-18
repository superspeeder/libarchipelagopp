//
// Created by andy on 10/17/2026.
//

#pragma once

#include "ap/clientinfo.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
namespace ssl       = boost::asio::ssl;
using tcp           = boost::asio::ip::tcp;

namespace ap {
    struct RoomConnectionInfo {
        std::string host = "archipelago.gg";
        uint16_t    port;
        std::string slot_name;
        std::string password;
    };

    class ClientConnectionBase {
      public:
        virtual ~ClientConnectionBase() = default;

        /**
         * @brief Attempt to connect to the archipelago server and perform the handshake.
         * @return If the connection was successful.
         */
        virtual bool handshake() = 0;

        virtual void        send_message_utf8(const std::string &message) = 0;
        virtual std::string read_message_utf8()                           = 0;
    };

    template <class T>
    class ClientConnection : public ClientConnectionBase {
        using stream_t = websocket::stream<T>;

        struct _empty {};

        using ssl_ctx_t = std::conditional_t<std::is_same_v<ssl::stream<beast::tcp_stream>, T>, ssl::context, _empty>;

      public:
        explicit ClientConnection(RoomConnectionInfo info);

        ~ClientConnection() override {
            if (_ws.is_open()) {
                _ws.close(websocket::close_code::normal);
            }
        }

        ClientConnection(const ClientConnection &other)                = delete;
        ClientConnection(ClientConnection &&other) noexcept            = delete;
        ClientConnection &operator=(const ClientConnection &other)     = delete;
        ClientConnection &operator=(ClientConnection &&other) noexcept = delete;

        bool        handshake() override;
        void        send_message_utf8(const std::string &message) override;
        std::string read_message_utf8() override;

      private:
        net::io_context _ioc;
        ssl_ctx_t       _ssl;
        tcp::resolver   _resolver;
        stream_t        _ws;

        bool               _is_connected = false;
        RoomConnectionInfo _active_connection_info;

        ClientInfo _client_info;
    };

    using ClientConnectionInsecure = ClientConnection<tcp::socket>;
    using ClientConnectionSecure   = ClientConnection<ssl::stream<beast::tcp_stream>>;

    class ClientConnectionAuto : public ClientConnectionBase {
      public:
        explicit ClientConnectionAuto(RoomConnectionInfo info);

        [[nodiscard]] bool handshake() override;

        [[nodiscard]] bool is_connected() const { return _is_connected; }

        [[nodiscard]] bool is_secure() const noexcept { return _is_secure; }

        void        send_message_utf8(const std::string &message) override;
        std::string read_message_utf8() override;

      private:
        bool                                  _is_secure;
        bool                                  _is_connected;
        std::unique_ptr<ClientConnectionBase> _active_connection;
        RoomConnectionInfo                    _connection_info;
    };
} // namespace ap
