ftp_include = include_directories(['include'])

# TODO: target properties
ftp = shared_library('mavsdk_ftp',
  ['ftp.cpp', 'ftp_impl.cpp', 'fs.cpp', 'crc32.cpp'],
  include_directories: [core_include, mavlink_include, ftp_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [ftp],
  subdirs: ['mavsdk/plugins/ftp', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_ftp',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/ftp/ftp.h',
  subdir: 'mavsdk/plugins/ftp')
