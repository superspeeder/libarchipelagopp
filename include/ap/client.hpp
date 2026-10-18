//
// Created by andy on 10/17/2026.
//

#pragma once
#include "ap/connection.hpp"
#include "ap/data_package_cache.hpp"

#include <boost/json.hpp>
#include <memory>

namespace ap {
    class ArchipelagoClient {
      public:
        ArchipelagoClient();

        [[nodiscard]] bool               connect(RoomConnectionInfo info);
        void                             send_json_raw(const boost::json::value &value) const;
        [[nodiscard]] boost::json::value read_json_raw() const;



      private:
        std::unique_ptr<ClientConnectionAuto>       _active_connection;
        std::unique_ptr<internal::DataPackageCache> _dpkg_cache;

        std::string slot_name;
        int64_t     slot_num;
        std::string game_name;


        bool _finish_connection();
    };
} // namespace ap
