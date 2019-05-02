#!/usr/bin/env groovy

pipeline {
  agent none

  stages {
    stage('Build') {
      parallel {

        stage('Ubuntu 16.04 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/debug -H.'
            sh 'make -Cbuild/debug install'
            sh 'build/debug/src/unit_tests_runner'
            sh 'build/debug/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('Ubuntu 16.04 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/release -H.'
            sh 'make -Cbuild/release install'
            sh 'build/release/src/unit_tests_runner'
            sh 'build/release/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('Ubuntu 18.04 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-18.04:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/debug -H.'
            sh 'make -Cbuild/debug install'
            sh 'build/debug/src/unit_tests_runner'
            sh 'build/debug/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('Ubuntu 18.04 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-18.04:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/release -H.'
            sh 'make -Cbuild/release install'
            sh 'build/release/src/unit_tests_runner'
            sh 'build/release/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('Fedora 27 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-28:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/debug -H.'
            sh 'make -Cbuild/debug install'
            sh 'build/debug/src/unit_tests_runner'
            sh 'build/debug/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('Fedora 27 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-28:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/release -H.'
            sh 'make -Cbuild/release install'
            sh 'build/release/src/unit_tests_runner'
            sh 'build/release/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('Fedora 28 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-29:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/debug -H.'
            sh 'make -Cbuild/debug install'
            sh 'build/debug/src/unit_tests_runner'
            sh 'build/debug/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('Fedora 28 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-29:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/release -H.'
            sh 'make -Cbuild/release install'
            sh 'build/release/src/unit_tests_runner'
            sh 'build/release/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('macOS Debug') {
          agent {
            label 'mac'
          }
          environment {
            CCACHE_BASEDIR = "${env.WORKSPACE}"
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/debug -H.'
            sh 'make -Cbuild/debug install'
            sh 'build/debug/src/unit_tests_runner'
            sh 'build/debug/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

        stage('macOS Release') {
          agent {
            label 'mac'
          }
          environment {
            CCACHE_BASEDIR = "${env.WORKSPACE}"
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule sync --recursive'
            sh 'git submodule update --init --recursive --force'
            sh 'ccache -z'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BACKEND=ON -DCMAKE_INSTALL_PREFIX=install -DENABLE_MAVLINK_PASSTHROUGH=ON -Bbuild/release -H.'
            sh 'make -Cbuild/release install'
            sh 'build/release/src/unit_tests_runner'
            sh 'build/release/src/backend/test/unit_tests_backend'
            sh 'cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${WORKSPACE}/install;${WORKSPACE}/third_party/build/install" -B./example/build -Hexample'
            sh 'make -Cexample/build'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }

      } // parallel
    } // Build

    stage('Test') {
      parallel {

        stage('check style') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2019-03-25'
              args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
            }
          }
          steps {
            sh 'export'
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh '${WORKSPACE}/tools/fix_style.sh ${WORKSPACE}'
          }
          post {
            always {
              sh 'ccache -s'
              sh 'git submodule deinit -f .'
              sh 'git clean -ff -x -d .'
            }
          }
        }
      } // parallel
    } // stage Test

    stage('Generate Docs') {
      agent {
        docker {
          image 'dronecode/dronecode-sdk-ubuntu-16.04:2019-03-25'
          args '-e CCACHE_BASEDIR=$WORKSPACE -v ${CCACHE_DIR}:${CCACHE_DIR}:rw'
        }
      }
      steps {
        sh 'export'
        sh 'git submodule deinit -f .'
        sh 'git clean -ff -x -d .'
        sh 'git submodule sync --recursive'
        sh 'git submodule update --init --recursive --force'
        sh '${WORKSPACE}/tools/generate_docs.sh'
      }
      post {
        always {
          sh 'git submodule deinit -f .'
          sh 'git clean -ff -x -d .'
        }
      }
    } // stage Generate Docs

  } // stages

  environment {
    CCACHE_DIR = '/tmp/ccache'
    XDG_CACHE_HOME = '/tmp/.cache' // For go build cache.
  }

  options {
    buildDiscarder(logRotator(numToKeepStr: '10', artifactDaysToKeepStr: '30'))
    timeout(time: 60, unit: 'MINUTES')
  }
}
