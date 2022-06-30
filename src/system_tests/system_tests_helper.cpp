#include "system_tests_helper.h"

namespace mavsdk {

std::future<std::shared_ptr<System>> wait_for_first_system_detected(Mavsdk& mavsdk)
{
    LogInfo() << "Waiting to discover system";

    auto prom = std::make_shared<std::promise<std::shared_ptr<System>>>();

    // We pass a copy of the shared ptr to the promise to keep it alive after
    // this function goes out of scope.
    const Mavsdk::NewSystemHandle handle =
        mavsdk.subscribe_on_new_system([&mavsdk, prom, handle]() {
            const auto system = mavsdk.systems().at(0);
            if (system->is_connected()) {
                mavsdk.unsubscribe_on_new_system(handle);
                LogInfo() << "Discovered system";
                prom->set_value(system);
            }
        });

    return prom->get_future();
}

} // namespace mavsdk
