#include "info.h"
#include <unistd.h>

namespace dronelink {

class InfoImpl {
public:
    InfoImpl();
    ~InfoImpl();

    bool is_complete() const;
    unsigned get_version() const;
    uint64_t get_uuid() const;

    void set_version(unsigned version);
    void set_uuid(uint64_t uuid);

private:
    unsigned _version;
    uint64_t _uuid;
};

} // namespace dronelink
