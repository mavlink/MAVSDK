conf_data = configuration_data()
conf_data.set('version', version_str)

configure_file(
  input: 'version.h.meson.in',
  output: 'version.h',
  configuration: conf_data,
)

threads = dependency('threads')

mavsdk_sources = [
  'call_every_handler.cpp',
  'connection.cpp',
  'connection_result.cpp',
  'curl_wrapper.cpp',
  'system.cpp',
  'system_impl.cpp',
  'mavsdk.cpp',
  'mavsdk_impl.cpp',
  'global_include.cpp',
  'http_loader.cpp',
  'mavlink_channels.cpp',
  'mavlink_commands.cpp',
  'mavlink_mission_transfer.cpp',
  'mavlink_parameters.cpp',
  'mavlink_receiver.cpp',
  'mavlink_message_handler.cpp',
  'plugin_impl_base.cpp',
  'serial_connection.cpp',
  'tcp_connection.cpp',
  'timeout_handler.cpp',
  'udp_connection.cpp',
  'log.cpp',
  'cli_arg.cpp',
  'geometry.cpp',
  'timesync.cpp',
]

#message(curl.dependency('curl'))

#message(curl.target_type('curl'))
#message('CMaket targets:\n - ' + '\n - '.join(curl.target_list()))
#message(curl.target_type('curl'))
mavsdk_lib = shared_library('mavsdk',
  mavsdk_sources,
  include_directories: mavlink_include,
  dependencies: [threads, curl.dependency('curl')],
  install: true)

install_headers([
  'connection_result.h',
  'system.h',
  'mavsdk.h',
  'plugin_base.h',
  'geometry.h',
])
