# core and some plugins require curl
if(ANDROID)
    set(CURL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/core/third_party/curl-android-ios/prebuilt-with-ssl/android/include)
    set(CURL_LIBRARY ${CMAKE_SOURCE_DIR}/core/third_party/curl-android-ios/prebuilt-with-ssl/android/${ANDROID_ABI}/libcurl.a)
elseif(IOS)
    set(CURL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/core/third_party/curl-android-ios/prebuilt-with-ssl/iOS/include)
    set(CURL_LIBRARY ${CMAKE_SOURCE_DIR}/core/third_party/curl-android-ios/prebuilt-with-ssl/iOS/libcurl.a)
elseif(MSVC)
    # You need to call cmake with -DWIN_CURL_INCLUDE_DIR:STRING="C:\\curl-7.54.1\\include"
    if(NOT WIN_CURL_INCLUDE_DIR)
        message(FATAL_ERROR "Please provide argument -DWIN_CURL_INCLUDE_DIR:STRING=\"path_to_curl_include\"")
    endif()
    if(NOT WIN_CURL_LIB)
        message(FATAL_ERROR "Please provide argument -DWIN_CURL_LIBSTRING=\"path_to_curl_lib\"")
    endif()

    set(CURL_INCLUDE_DIRS ${WIN_CURL_INCLUDE_DIR})
    set(CURL_LIBRARY ${WIN_CURL_LIB})

    add_definitions(
        -DCURL_STATICLIB
    )
else()
    find_package(CURL REQUIRED)
    set(CURL_LIBRARY "curl")
endif()
