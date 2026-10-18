//
// Created by andy on 10/17/2026.
//
#include "ap/connection.hpp"

#include <iostream>
#include <utility>

namespace ap {
    template <>
    ClientConnectionInsecure::ClientConnection(RoomConnectionInfo info) : _resolver{_ioc}, _ws{_ioc}, _active_connection_info(std::move(info)) {}

    template <>
    ClientConnectionSecure::ClientConnection(RoomConnectionInfo info)
        : _ssl{ssl::context::tlsv12_client}, _resolver{_ioc}, _ws{_ioc, _ssl}, _active_connection_info(std::move(info)) {}

    template <>
    bool ClientConnectionInsecure::handshake() {
        auto const  address_info = _resolver.resolve(_active_connection_info.host, std::to_string(_active_connection_info.port), net::ip::resolver_base::numeric_service);
        const std::string host_string  = _active_connection_info.host + ":" + std::to_string(_active_connection_info.port);

        try {
            net::connect(beast::get_lowest_layer(_ws), address_info);
            websocket::permessage_deflate opt;
            opt.client_enable = true;
            opt.server_enable = true;
            _ws.set_option(opt);
            _ws.set_option(websocket::stream_base::decorator([](websocket::request_type &req) {
                req.set(beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro libarchipelagopp");
            }));

            boost::system::error_code ec;
            _ws.handshake(host_string, "/", ec);
            if (ec.failed()) {
                std::cerr << "Error connecting to wss://" << host_string << ": " << ec.what() << std::endl;
                return false;
            }

        } catch (std::exception &e) {
            std::cerr << "Error connecting to ws://" << host_string << ": " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    template <>
    bool ClientConnectionSecure::handshake() {
        auto const  address_info = _resolver.resolve(_active_connection_info.host, std::to_string(_active_connection_info.port), net::ip::resolver_base::numeric_service);
        const std::string host_string  = _active_connection_info.host + ":" + std::to_string(_active_connection_info.port);

        try {
            net::connect(beast::get_lowest_layer(_ws).socket(), address_info);
            websocket::permessage_deflate opt;
            opt.client_enable = true;
            opt.server_enable = true;
            _ws.set_option(opt);
            _ws.set_option(websocket::stream_base::decorator([](websocket::request_type &req) {
                req.set(beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro libarchipelagopp");
            }));

            boost::system::error_code ec;
            _ws.next_layer().handshake(ssl::stream_base::client, ec);
            if (ec.failed()) {
                std::cerr << "Error connecting to wss://" << host_string << ": " << ec.what() << std::endl;
                return false;
            }
            _ws.handshake(host_string, "/", ec);
            if (ec.failed()) {
                std::cerr << "Error connecting to wss://" << host_string << ": " << ec.what() << std::endl;
                return false;
            }
        } catch (std::exception &e) {
            std::cerr << "Error connecting to wss://" << host_string << ": " << e.what() << std::endl;
            return false;
        }

        return true;
    }

    template <>
    void ClientConnectionSecure::send_message_utf8(const std::string &message) {
        _ws.write(net::buffer(message));
    }

    template <>
    std::string ClientConnectionSecure::read_message_utf8() {
        beast::flat_buffer buffer;
        _ws.read(buffer);
        std::string str{static_cast<const char *>(buffer.cdata().data()), buffer.cdata().size()};
        return std::move(str);
    }

    template <>
    void ClientConnectionInsecure::send_message_utf8(const std::string &message) {
        _ws.write(net::buffer(message));
    }

    template <>
    std::string ClientConnectionInsecure::read_message_utf8() {
        beast::flat_buffer buffer;
        _ws.read(buffer);
        std::string str{static_cast<const char *>(buffer.cdata().data()), buffer.cdata().size()};
        return std::move(str);
    }

    void ClientConnectionAuto::send_message_utf8(const std::string &message) {
        _active_connection->send_message_utf8(message);
    }

    std::string ClientConnectionAuto::read_message_utf8() {
        auto str = _active_connection->read_message_utf8();
        return std::move(str);
    }

    ClientConnectionAuto::ClientConnectionAuto(RoomConnectionInfo info) : _is_secure(false), _is_connected(false), _connection_info(std::move(info)) {}

    bool ClientConnectionAuto::handshake() {
        _active_connection = std::make_unique<ClientConnectionSecure>(_connection_info);
        _is_secure         = _active_connection->handshake();
        _is_connected      = _is_secure;
        if (!_is_secure) {
            _active_connection = std::make_unique<ClientConnectionInsecure>(_connection_info);
            _is_connected      = _active_connection->handshake();
        }
        return _is_connected;
    }
} // namespace ap
