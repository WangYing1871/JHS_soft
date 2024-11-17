#What
  JianWei JHS analysis codes: unapck; baseline; fec2det; reco

#language
  CMake
  C++

#3rd-party
  CERN-Root https://root.cern/releases/release-63204/

#DIRECTORY
  inc: project head files
  src: project source files
  script: view

#How To Make
  cd path/to/this/project
  mkdir build && cd build && cmake ../ && make -j4


#Parameter
  this project parameters transfer by "argv.ini"
  'unpack/Enable'  is enable this step
  'unpack/filename' raw dat filename
  'unapck/fec-count' fec board count

  'config/Enable' same as unapck
  'config/prestal-path' prestal file path for write

  'map/enable' same sa unapck
  'map/filename' unapcked root file by 'unpack'
  'map/mapname' fec channel to det channel map
  'map/prestal' baseline file

#AUTHOR(s)
  wangying@JW   e-mail: 2835516101@qq.com
