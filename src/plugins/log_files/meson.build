log_files_include = include_directories('include')

# TODO: target properties
log_files = shared_library('mavsdk_log_files',
  ['log_files.cpp', 'log_files_impl.cpp'],
  include_directories: [core_include, mavlink_include, log_files_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/log_files/log_files.h')
