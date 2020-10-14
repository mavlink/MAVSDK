project('takeoff_and_land', 'cpp')

mavsdk = dependency('mavsdk')
mavsdk_action = dependency('mavsdk_action')
mavsdk_telemetry = dependency('mavsdk_telemetry')

executable('takeoff_and_land',
  'takeoff_and_land.cpp',
  dependencies: [mavsdk, mavsdk_action, mavsdk_telemetry]
)
