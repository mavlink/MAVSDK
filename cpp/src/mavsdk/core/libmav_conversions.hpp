#pragma once

#include <string>

namespace mav {
class Message;
}

namespace mavsdk {

/**
 * @brief Populate the fields of a libmav message from a JSON object string.
 *
 * The message must already have been created from the MessageSet (which defines
 * its fields). Each member of the JSON object is set on the message, with the
 * value type (integer, float, string, array) matched to the field definition.
 *
 * This is shared by the MavlinkDirect (client) and MavlinkDirectServer (server)
 * plugins so that both encode messages identically.
 *
 * @return false only if the JSON string cannot be parsed. Individual field set
 *         failures are logged and tolerated (they do not fail the whole message).
 */
bool json_to_libmav_message(const std::string& json_string, mav::Message& msg);

} // namespace mavsdk
