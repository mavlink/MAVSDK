%module dronelink
%{
    #define SWIG_FILE_WITH_INIT
    #include "../include/dronelink.h"
    #include "../core/plugin_base.h"
    #include "../build/default/include/device_plugin_container.h"
    #include "../include/device.h"
    #include "../plugins/action/action.h"
    #include "../plugins/info/info.h"
    #include "../plugins/logging/logging.h"
    #include "../plugins/mission/mission.h"
    #include "../plugins/mission/mission_item.h"
    #include "../plugins/telemetry/telemetry.h"
    using namespace dronelink;
%}

%include "../include/dronelink.h";
%include "../core/plugin_base.h"
%include "../build/default/include/device_plugin_container.h";
%include "../include/device.h"
%include "../plugins/action/action.h"
%include "../plugins/info/info.h"
%include "../plugins/logging/logging.h"
%include "../plugins/mission/mission.h"
%include "../plugins/mission/mission_item.h"
%include "../plugins/telemetry/telemetry.h"
