project('mavsdk', 'cpp')

add_global_arguments('-DLINUX', language: 'cpp')

r = run_command('git', 'describe', '--abbrev=8', '--dirty', '--always', '--tags')
version_str = r.stdout().strip()

cmake = import('cmake')

if get_option('build_deps')
  jsoncpp = subproject('jsoncpp')
  tinyxml2 = subproject('tinyxml2')
  zlib = subproject('zlib')
  curl = cmake.subproject('curl')
  libressl = cmake.subproject('libressl')
  cares = cmake.subproject('cares')
  protobuf = subproject('protobuf')
  absl = subproject('abseil-cpp')
  grpc = cmake.subproject('grpc')
endif

subdir('src')
