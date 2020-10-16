project('mavsdk', 'cpp')

add_global_arguments('-DLINUX', language: 'cpp')

r = run_command('git', 'describe', '--abbrev=8', '--dirty', '--always', '--tags')
version_str = r.stdout().strip()

cmake = import('cmake')
pkg = import('pkgconfig')

thread_dep = dependency('threads')

if get_option('build_deps')
  jsoncpp = subproject('jsoncpp')
  jsoncpp_dep = jsoncpp.get_variable('jsoncpp_dep')
  tinyxml2 = subproject('tinyxml2')
  tinyxml2_dep = tinyxml2.get_variable('tinyxml2_dep')
  zlib = subproject('zlib')
  curl_dep = dependency('libcurl')
  libressl = cmake.subproject('libressl')
  cares = cmake.subproject('cares')
  protobuf = subproject('protobuf')
  absl = subproject('abseil-cpp')
  grpc = cmake.subproject('grpc')
endif

subdir('src')
