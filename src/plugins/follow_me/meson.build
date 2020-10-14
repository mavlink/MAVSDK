follow_me_include = include_directories('include')

# TODO: target properties
follow_me = shared_library('mavsdk_follow_me',
  ['follow_me.cpp', 'follow_me_impl.cpp'],
  include_directories: [core_include, mavlink_include, follow_me_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/follow_me/follow_me.h')
