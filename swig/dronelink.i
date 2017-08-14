%module dronelink
%{
    #define SWIG_FILE_WITH_INIT
    #include "../include/dronelink.h"
    using namespace dronelink;
%}

%include "../include/dronelink.h";
