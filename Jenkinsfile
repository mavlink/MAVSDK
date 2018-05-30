pipeline {
  agent any
  stages {
    stage('Build') {
      parallel {
        stage('Ubuntu 16.04 Debug') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-16.04'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug'
          }
        }
        stage('Ubuntu 16.04 Release') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-16.04'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release'
          }
        }
        stage('Ubuntu 18.04 Debug') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-18.04'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug'
          }
        }
        stage('Ubuntu 18.04 Release') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-18.04'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release'
          }
        }
        stage('Fedora 27 Debug') {
          agent {
            docker {
              image 'dronecore/dronecore-fedora-27'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug'
          }
        }
        stage('Fedora 27 Release') {
          agent {
            docker {
              image 'dronecore/dronecore-fedora-27'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release'
          }
        }
        stage('Fedora 28 Debug') {
          agent {
            docker {
              image 'dronecore/dronecore-fedora-28'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug'
          }
        }
        stage('Fedora 28 Release') {
          agent {
            docker {
              image 'dronecore/dronecore-fedora-28'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release'
          }
        }
      }
    }
    stage('Test') {
      parallel {
        stage('check style') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-16.04'
            }
          }
          steps {
            sh 'make fix_style'
          }
        }
        stage('example/takeoff_land') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-16.04'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'mkdir -p example/takeoff_land/build'
            sh 'cd example/takeoff_land/build && cmake ..'
            // FIXME sh 'make -C example/takeoff_land/build'
          }
        }
        stage('example/fly_mission') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-16.04'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'mkdir -p example/fly_mission/build'
            sh 'cd example/fly_mission/build && cmake ..'
            // FIXME sh 'make -C example/fly_mission/build'
          }
        }
        stage('example/offboard_velocity') {
          agent {
            docker {
              image 'dronecore/dronecore-ubuntu-16.04'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'mkdir -p example/offboard_velocity/build'
            sh 'cd example/offboard_velocity/build && cmake ..'
            // FIXME sh 'make -C example/offboard_velocity/build'
          }
        }
      }
    }
    stage('Generate Docs') {
      agent {
        docker {
          image 'dronecore/dronecore-ubuntu-16.04'
        }
      }
      steps {
        sh 'git submodule deinit -f .'
        sh 'git clean -ff -x -d .'
        sh 'git submodule update --init --recursive --force'
        sh './generate_docs.sh'
      }
    }
  }
}
