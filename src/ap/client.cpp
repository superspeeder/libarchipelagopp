//
// Created by andy on 10/17/2026.
//
#include "ap/client.hpp"
#include "ap/connection.hpp"

namespace ap {
    ArchipelagoClient::ArchipelagoClient() = default;

    bool ArchipelagoClient::connect(RoomConnectionInfo info) {
        _active_connection = std::make_unique<ClientConnectionAuto>(std::move(info));
        bool status = _active_connection->handshake();
        if (status) {
            status = _finish_connection();
        }

        return status;
    }

    void ArchipelagoClient::send_json_raw(const boost::json::value &value) const {
        const std::string string = boost::json::serialize(value);
        _active_connection->send_message_utf8(string);
    }

    boost::json::value ArchipelagoClient::read_json_raw() const {
        const auto data = _active_connection->read_message_utf8();
        return boost::json::parse(data);
    }

    bool ArchipelagoClient::_finish_connection() {

    }
} // namespace ap
