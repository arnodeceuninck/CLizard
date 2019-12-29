python "grammar/Simple Grammar/txtToJson.py" "grammar/Simple Grammar/grammar.txt" "grammar/Simple Grammar/cpp.json"

# Update cmake
sudo apt remove cmake
wget https://github.com/Kitware/CMake/releases/download/v3.16.2/cmake-3.16.2-Linux-x86_64.sh
sudo mv cmake-3.16.2-Linux-x86_64.sh /opt/
chmod +x /opt/cmake-3.16.2-Linux-x86_64.sh
cd /opt
sudo bash /opt/cmake-3.16.2-Linux-x86_64.sh
sudo ln -s /opt/cmake-3.16.2-Linux-x86_64/bin/* /usr/local/bin
cd -

# Compile
cmake
make


#g++ -std=c++17 src/*.cpp -o clizard # TODO: Fix selection of the right input files



sudo: false
dist: trusty
language: cpp
compiler:
  - gcc

# credits to Sam Legrand
before_install:
  - sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
  - sudo apt-get update
  - sudo apt-get install gcc-7 g++-7
  - gcc-7 --version
  - mkdir cmake-dist
  - travis_retry wget -qO- https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz | sudo tar --strip-components=1 -C cmake-dist -xz
  - export PATH=$PWD/cmake-dist/bin:$PATH;

install:
  - if [ "$CXX" = "g++" ]; then export CXX="g++-7" CC="gcc-7"; fi

script:
  - mkdir build
  - cd build
  - gcc -v && g++ -v && cmake --version
  - cmake -DCMAKE_CXX_COMPILER=$COMPILER .. && make