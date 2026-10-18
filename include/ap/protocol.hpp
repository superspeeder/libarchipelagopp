//
// Created by andy on 10/17/2026.
//

#pragma once
#include <boost/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ap::proto {
    enum NetworkItemFlags {
        NETWORK_ITEM_FLAG_UNIMPORTANT         = 0b000,
        NETWORK_ITEM_FLAG_LOGICAL_ADVANCEMENT = 0b001,
        NETWORK_ITEM_FLAG_USEFUL              = 0b010,
        NETWORK_ITEM_FLAG_TRAP                = 0b100
    };

    enum ClientStatus { CLIENT_STATUS_UNKNOWN = 0, CLIENT_STATUS_CONNECTED = 5, CLIENT_STATUS_READY = 10, CLIENT_STATUS_PLAYING = 20, CLIENT_STATUS_GOAL = 30 };

    enum SlotType {
        SLOT_TYPE_SPECTATOR = 0b00,
        SLOT_TYPE_PLAYER    = 0b01,
        SLOT_TYPE_GROUP     = 0b10,
    };

    enum Permission {
        PERMISSION_DISABLED     = 0b000,
        PERMISSION_ENABLED      = 0b001,
        PERMISSION_GOAL         = 0b010,
        PERMISSION_AUTO         = 0b110,
        PERMISSION_AUTO_ENABLED = 0b111,
    };

    enum HintStatus {
        HINT_STATUS_UNSPECIFIED = 0,
        HINT_STATUS_NO_PRIORITY = 10,
        HINT_AVOID              = 20,
        HINT_PRIORITY           = 30,
        HINT_FOUND              = 40,
    };

    struct NetworkPlayer {
        int64_t     team, slot;
        std::string alias, name;
    };

    struct NetworkItem {
        int64_t item, location, player, flags;
    };

    struct JSONMessagePart {
        std::optional<std::string> type;
        std::optional<std::string> text;
        std::optional<std::string> color; // TODO: handle color parsing.
        std::optional<uint32_t>    flags;
        std::optional<int64_t>     player;
        std::optional<HintStatus>  hint_status;
    };

    struct NetworkVersion {
        int64_t major, minor, build;
    };

    struct NetworkSlot {
        std::string          name;
        std::string          game;
        SlotType             type;
        std::vector<int64_t> group_members;
    };

    struct Hint {
        int64_t     receiving_player;
        int64_t     finding_player;
        int64_t     location;
        int64_t     item;
        bool        found;
        std::string entrance;
        int         item_flags;
        HintStatus  status = HINT_STATUS_UNSPECIFIED;
    };

    namespace dpkg {
        struct GameData {
            std::unordered_map<std::string, int64_t> item_name_to_id;
            std::unordered_map<std::string, int64_t> location_name_to_id;
            std::string                              checksum;
        };

        struct DataPackage {
            std::unordered_map<std::string, GameData> games;
        };
    } // namespace dpkg

    namespace s2c {
        struct RoomInfo {
            NetworkVersion                               version, generator_version;
            std::vector<std::string>                     tags;
            bool                                         password;
            std::unordered_map<std::string, Permission>  permissions;
            int64_t                                      hint_cost;
            int64_t                                      location_check_points;
            std::vector<std::string>                     games;
            std::unordered_map<std::string, std::string> datapackage_checksums;
            std::string                                  seed_name;
            double                                       time;
        };

        struct ConnectionRefused {
            std::vector<std::string> errors;
        };

        struct Connected {
            int64_t                                  team;
            int64_t                                  slot;
            std::vector<NetworkPlayer>               players;
            std::vector<int64_t>                     missing_locations;
            std::vector<int64_t>                     checked_locations;
            boost::json::object                      slot_data;
            std::unordered_map<int64_t, NetworkSlot> slot_info;
            int64_t                                  hint_points;
        };

        struct ReceivedItems {
            int64_t                  index;
            std::vector<NetworkItem> items;
        };

        struct LocationInfo {
            std::vector<NetworkItem> locations;
        };

        struct RoomUpdate : RoomInfo {
            std::vector<NetworkPlayer> players;
            std::vector<int64_t>       checked_locations;
        };

        struct RoomUpdateAuthenticated : RoomUpdate, Connected {};

        struct PrintJSON {
            std::vector<JSONMessagePart>            data;
            std::string                             type;
            std::optional<int64_t>                  receiving;
            std::optional<NetworkItem>              item;
            std::optional<bool>                     found;
            std::optional<int64_t>                  team;
            std::optional<int64_t>                  slot;
            std::optional<std::string>              message;
            std::optional<std::vector<std::string>> tags;
            std::optional<int64_t>                  countdown;
        };

        struct DataPackage {
            boost::json::object
                data; // Purposefully not including the definition of this data here because it is complex and does not fit the model I want to use for developing this.
        };

        struct Bounced {
            std::vector<int64_t>     teams;
            std::vector<std::string> games;
            std::vector<int64_t>     slots;
            std::vector<std::string> tags;
            std::string              operator_;
            boost::json::object      data;
        };

        struct InvalidPacket {
            std::string                type;
            std::optional<std::string> original_cmd;
            std::string                text;
        };

        struct Retrieved {
            boost::json::object keys;
        };

        struct SetReply {
            std::string        key;
            boost::json::value value;
            boost::json::value original_value;
            int64_t            slot;
        };
    } // namespace s2c

    namespace c2s {
        struct Connect {
            std::string              password;
            std::string              game;
            std::string              name;
            std::string              uuid;
            NetworkVersion           version;
            int                      items_handling;
            std::vector<std::string> tags;
            bool                     slot_data;
        };

        enum ItemsHandlingFlags {
            ITEMS_HANDLING_NONE              = 0b000,
            ITEMS_HANDLING_FROM_OTHER_WORLDS = 0b001,
            ITEMS_HANDLING_FROM_OWN          = 0b010,
            ITEMS_HANDLING_STARTING_INV      = 0b100,
        };

        struct ConnectUpdate {
            int                      items_handling;
            std::vector<std::string> tags;
        };

        struct Sync {};

        struct LocationChecks {
            std::vector<int64_t> locations;
        };

        struct LocationScouts {
            std::vector<int64_t> locations;
            int                  create_as_hint;
        };

        struct CreateHints {
            std::vector<int64_t> locations;
            int64_t              player;
            HintStatus           status;
        };

        struct UpdateHint {
            int64_t    player;
            int64_t    location;
            HintStatus status;
        };

        struct StatusUpdate {
            ClientStatus status;
        };

        struct Say {
            std::string text;
        };

        struct GetDataPackage {
            std::vector<std::string> games;
        };

        struct Bounce {
            std::vector<int64_t>     teams;
            std::vector<std::string> games;
            std::vector<int64_t>     slots;
            std::vector<std::string> tags;
            std::string              operator_;
            boost::json::object      data;
        };

        struct Get {
            std::vector<std::string>           keys;
            std::optional<boost::json::object> extra_arguments;
        };

        enum class DataStorageOperator { Replace, Default, Add, Mul, Pow, Mod, Floor, Ceil, Max, Min, And, Or, Xor, LeftShift, RightShift, Remove, Pop, Update };

        struct DataStorageOperation {
            DataStorageOperator operation;
            boost::json::value  value;
        };

        struct Set {
            std::string                       key;
            boost::json::value                default_;
            bool                              want_reply;
            std::vector<DataStorageOperation> operations;
        };

        struct SetNotify {
            std::vector<std::string> keys;
        };
    } // namespace c2s
} // namespace ap::proto
