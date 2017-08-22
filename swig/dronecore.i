%module dronecore
%feature("autodoc", "3");

%{
    #define SWIG_FILE_WITH_INIT
    #include "../include/dronecore.h"
    #include "../core/plugin_base.h"
    #include "../build/android_x86/include/device_plugin_container.h"
    #include "../include/device.h"
    #include "../plugins/action/action.h"
    #include "../plugins/info/info.h"
    #include "../plugins/logging/logging.h"
    #include "../plugins/mission/mission.h"
    #include "../plugins/mission/mission_item.h"
    #include "../plugins/telemetry/telemetry.h"
    using namespace dronecore;
%}

%include <typemaps.i>
%include "../include/dronecore.h";
%include "../core/plugin_base.h"
%include "../build/android_x86/include/device_plugin_container.h";
%include "../include/device.h"
%include "../plugins/action/action.h"
%include "../plugins/info/info.h"
%include "../plugins/logging/logging.h"
%include "../plugins/mission/mission.h"
%include "../plugins/mission/mission_item.h"
%include "../plugins/telemetry/telemetry.h"
