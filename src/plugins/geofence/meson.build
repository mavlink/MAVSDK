geofence_include = include_directories('include')

# TODO: target properties
geofence = shared_library('mavsdk_geofence',
  ['geofence.cpp', 'geofence_impl.cpp'],
  include_directories: [core_include, mavlink_include, geofence_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/geofence/geofence.h')
