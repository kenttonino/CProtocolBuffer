## Description

> - Learning gRPC with Protobuf and C/C++.

<br />
<br />
<br />

## Local Setup

> - Before you proceed, you need to clone the [gRPC](https://github.com/grpc/grpc) codebase and build it from source.
> - Below are the steps on how to build it from source (tested in Debian Linux).

```sh
# (1) Install the necessary dependencies.
sudo apt install build-essential autoconf libtool pkg-config cmake

# (Optional) If you want to contribute.
sudo apt install clang libc++-dev

# (2) Clone the codebase.
git clone git@github.com:grpc/grpc.git

# (3) After cloning, download the submodules containing source codes for gRPC dependencies.
git submodule update --init

# (4) Create a cmake build directory.
mkdir -p cmake/build
cd cmake/build

# (5) Build the codebase.
cmake -DCMAKE_CXX_STANDARD=17 ../..
make

# (6) Then install it to your prefer directory.
sudo mkdir /opt/grpc
cmake --install . --prefix /opt/grpc

# (7) Export the gRPC headers in your bashrc.
export PATH="/opt/grpc/bin:$PATH"
```

> - To run the project, follow the scripts below or check the Makefile.

```sh
# Build the project.
make build

# Run the route server.
make run-server
```

<br />
<br />
<br />

## References

> - https://grpc.io/docs/languages/cpp/quickstart/
> - https://github.com/grpc/grpc/blob/master/BUILDING.md#build-from-source
> - https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html
