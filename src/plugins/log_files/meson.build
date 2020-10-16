log_files_include = include_directories(['include'])

# TODO: target properties
log_files = shared_library('mavsdk_log_files',
  ['log_files.cpp', 'log_files_impl.cpp'],
  include_directories: [core_include, mavlink_include, log_files_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [log_files],
  subdirs: ['mavsdk/plugins/log_files', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_log_files',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/log_files/log_files.h',
  subdir: 'mavsdk/plugins/log_files')
