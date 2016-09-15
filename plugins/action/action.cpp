#include "action.h"
#include "action_impl.h"

namespace dronelink {

Action::Action(ActionImpl *impl) :
    _impl(impl)
{
}

Action::~Action()
{
}

Action::Result Action::arm() const
{
    return _impl->arm();
}

Action::Result Action::disarm() const
{
    return _impl->disarm();
}

Action::Result Action::kill() const
{
    return _impl->kill();
}

Action::Result Action::takeoff() const
{
    return _impl->takeoff();
}

Action::Result Action::land() const
{
    return _impl->land();
}

Action::Result Action::return_to_land() const
{
    return _impl->return_to_land();
}

//void Action::arm_async_new(new_result_callback_t callback)
//{
//    _impl->arm_async_new(callback);
//}

void Action::arm_async(CallbackData &callback)
{
    _impl->arm_async(callback);
}

void Action::disarm_async(CallbackData &callback)
{
    _impl->disarm_async(callback);
}

void Action::kill_async(CallbackData &callback)
{
    _impl->kill_async(callback);
}

void Action::takeoff_async(CallbackData &callback)
{
    _impl->takeoff_async(callback);
}

void Action::land_async(CallbackData &callback)
{
    _impl->land_async(callback);
}

void Action::return_to_land_async(CallbackData &callback)
{
    _impl->return_to_land_async(callback);
}

const char *Action::result_str(Result result) {
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::NO_DEVICE:
            return "No device";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::BUSY:
            return "Busy";
        case Result::COMMAND_DENIED:
            return "Command denied";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
            return "Unknown";
    }
}


} // namespace dronelink
