//
// Created by andy on 10/17/2026.
//

#include "ap/client.hpp"

#include <iostream>

static const ap::RoomConnectionInfo REMOTE{.host = "archipelago.gg", .port = 52123, .slot_name = "andy", .password = ""};

static const ap::RoomConnectionInfo LOCAL{.host = "localhost", .port = 38281, .slot_name = "andy", .password = ""};

int main() {
    /// [{"cmd": "RoomInfo", "version": {"major": 0, "minor": 1, "build": 3, "class": "Version"}, "tags": ["WebHost"], ... }]
    ap::ArchipelagoClient client{};
    try {
        auto       _         = client.connect(REMOTE);
        const auto room_info = client.read_json_raw().as_array()[0];
        std::cout << room_info << std::endl;

        auto                             games = room_info.at("games").as_array();
        std::vector<boost::json::string> games_to_request;
        for (const auto &game : games) {
            if (game.as_string() != "Archipelago") {
                games_to_request.push_back(game.as_string());
            }
        }

        // boost::json::array request = {boost::json::object{{"cmd", "GetDataPackage"}, {"games", games_to_request}}};
        boost::json::array request = {boost::json::object{{"cmd", "Connect"},
                                                          {"game", "Dead Cells"},
                                                          {"name", "andy"},
                                                          {"uuid", "ef5bafb2-fdcc-4e14-a71e-73410f893c5c"},
                                                          {"version", boost::json::object{{"class", "Version"}, {"major", 0}, {"minor", 6}, {"build", 8}}},
                                                          {"items_handling", 0},
                                                          {"tags", boost::json::array{"Tracker"}},
                                                          {"slot_data", false},
                                                          {"password", ""}}};
        std::cout << request << std::endl;
        client.send_json_raw(request);
        auto dpkg = client.read_json_raw();
        std::cout << dpkg << std::endl;
    } catch (std::exception &e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
    }
}
