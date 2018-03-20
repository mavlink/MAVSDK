#pragma once

#include "plugin_impl_base.h"
#include "mavlink_include.h"
#include "system.h"
#include "mavlink_system.h"
#include "logging.h"

namespace dronecore {

class LoggingImpl : public PluginImplBase
{
public:
    LoggingImpl(System &system);
    ~LoggingImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Logging::Result start_logging() const;
    Logging::Result stop_logging() const;

    void start_logging_async(const Logging::result_callback_t &callback);
    void stop_logging_async(const Logging::result_callback_t &callback);

private:
    void process_logging_data(const mavlink_message_t &message);
    void process_logging_data_acked(const mavlink_message_t &message);

    static Logging::Result logging_result_from_command_result(MAVLinkCommands::Result result);

    static void command_result_callback(MAVLinkCommands::Result command_result,
                                        const Logging::result_callback_t &callback);
};

} // namespace dronecore
