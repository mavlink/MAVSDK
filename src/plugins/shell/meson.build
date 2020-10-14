shell_include = include_directories('include')

# TODO: target properties
shell = shared_library('mavsdk_shell',
  ['shell.cpp', 'shell_impl.cpp'],
  include_directories: [core_include, mavlink_include, shell_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/shell/shell.h')
