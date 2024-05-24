#include <cstdint>
#include <vector>
#include <string>

// Taken from https://stackoverflow.com/a/13935718/8548472

namespace mavsdk {

std::string base64_encode(std::vector<uint8_t>& raw);
std::vector<uint8_t> base64_decode(const std::string& str);

} // namespace mavsdk
