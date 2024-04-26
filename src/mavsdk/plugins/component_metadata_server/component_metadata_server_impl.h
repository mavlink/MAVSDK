#pragma once

#include "plugins/component_metadata_server/component_metadata_server.h"
#include "server_plugin_impl_base.h"
#include "callback_list.h"
#include <filesystem>

namespace mavsdk {

class ComponentMetadataServerImpl : public ServerPluginImplBase {
public:
    explicit ComponentMetadataServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~ComponentMetadataServerImpl() override;

    void init() override;
    void deinit() override;

    void set_metadata(const std::vector<ComponentMetadataServer::Metadata>& metadata);

private:
    struct Metadata {
        explicit Metadata(ComponentMetadataServer::MetadataType metadata_type, uint32_t crc_data);

        COMP_METADATA_TYPE type;
        std::string filename;
        uint32_t crc;
    };

    bool generate_component_metadata_general_file();
    std::optional<MAV_RESULT> process_component_metadata_requested();

    static constexpr const char* kComponentGeneralFilename = "comp_general.json";

    std::mutex _mutex{}; ///< protect concurrent access to any of the class members

    std::vector<Metadata> _metadata;
    bool _metadata_set{false};
    std::filesystem::path _tmp_path; ///< ftp root dir

    uint32_t _comp_info_general_crc{};
    bool _verbose_debugging{false};
};

} // namespace mavsdk
