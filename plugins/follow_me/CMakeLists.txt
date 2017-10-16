set(class_name
    FollowMe
    PARENT_SCOPE
)

set(source_files
    follow_me.cpp
    follow_me_impl.cpp
    PARENT_SCOPE
)

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

set(unittest_source_files
    follow_me_impl_test.cpp
    PARENT_SCOPE
)

set(header_files
    follow_me.h
    PARENT_SCOPE
)

set(impl_header_files
    follow_me_impl.h
    PARENT_SCOPE
)

# To include an additional library, use:
# set(additional_libs "some_library" PARENT_SCOPE)
