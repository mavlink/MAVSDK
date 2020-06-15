#include "node.h"
#include "global_include.h"
#include "plugin_impl_base.h"

namespace mavsdk {

PluginImplBase::PluginImplBase(Node& node) : _parent(node.node_impl()) {}

} // namespace mavsdk
