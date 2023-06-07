<!--
Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# QuickStart

This guide will help you set up your programming environment to successfully command and control the Boston Dynamics Spot robot using the Spot C++ SDK. The guide defaults to a Linux setup.

**Windows users:** Please find notes like this to help you where Windows may differ from Linux.

<!--ts-->

- [System setup](#system-setup)
  - [System requirements](#system-requirements)
  - [C++ requirements](#c-requirements)
  - [Install C++ SDK dependencies](#install-c-sdk-dependencies)
- [Install the C++ SDK](#install-the-c-sdk)
  - [Clone the SDK from GitHub](#clone-the-sdk-from-github)
  - [Compile and install the C++ SDK](#compile-and-install-the-c-sdk)
- [Verify you can command and query Spot](#verify-you-can-command-and-query-spot)
  - [Get a Spot robot](#get-a-spot-robot)
  - [Get a user account on the robot](#get-a-user-account-on-the-robot)
  - [Ping Spot](#ping-spot)
- [Run Hello Spot example](#run-hello_spot-example)
- [Next steps](#next-steps)
<!--te-->

## System setup

### System requirements

The Boston Dynamics Spot C++ SDK works with most operating systems including:

- Linux Ubuntu 18.04 LTS
  - gcc version 7+
- Windows 10
  - Microsoft Visual Studio 2017+
- cmake version 3.10.2+, 3.14+ for CORE I/O

On Linux, install all the system requirements with the command:

```cmd
sudo apt-get update && apt-get -y install build-essential git g++ pkg-config curl tar zip unzip zlib1g-dev libssl-dev
```

On Windows, install Microsoft Visual Studio and git manually.

### C++ requirements

Spot C++ SDK works with gcc and Microsoft Visual Studio.

### Install C++ SDK dependencies

The C++ SDK depends on gRPC, Protobuf, Eigen and CLI11. Protobuf is included in gRPC. The instructions below use `vcpkg` to install these dependencies. All commands need to be run on a Linux/Windows terminal.

1. Clone vcpkg. The checkout hash below specifies the one used for testing internally. This version of vcpkg installs the dependency versions:
   - gRPC 1.51.1
   - Protobuf 3.21.12
   - Eigen 3.4.0#2
   - CLI11 2.3.1

It also downloads cmake in the location `downloads/tools/{CMAKE_VERSION}/{CMAKE_VERSION}/bin/cmake`, which is needed to build the SDK. Please add that folder to your PATH to simplify running cmake on the terminal.

```cmd
git clone https://github.com/microsoft/vcpkg
cd vcpkg
git checkout c6592ce60ff394049905365865f59e3a4d93d35b
```

2. On x86-64 Linux:

```cmd
./bootstrap-vcpkg.sh
./vcpkg install grpc:x64-linux
./vcpkg install eigen3:x64-linux
./vcpkg install cli11:x64-linux
```

3. On arm64 Linux, such as CORE I/O:

```cmd
./bootstrap-vcpkg.sh
export VCPKG_FORCE_SYSTEM_BINARIES=arm
./vcpkg install grpc:arm64-linux
./vcpkg install eigen3:arm64-linux
./vcpkg install cli11:arm64-linux
```

4. On Windows, add `set(VCPKG_PLATFORM_TOOLSET v141)` in file `vcpkg\triplets\x64-windows.cmake` to get Visual Studio 2017 builds.

```cmd
> .\bootstrap-vcpkg.bat
> .\vcpkg.exe install grpc:x64-windows
> .\vcpkg.exe install eigen3:x64-windows
> .\vcpkg.exe install cli11:x64-windows
```

## Install the C++ SDK

The Spot C++ SDK contains cmake files with instructions to configure, build and install the SDK. The cmake system also auto-generates the C++ protobuf classes from the protobuf definitions in the Spot API.

### Clone the SDK from GitHub

Developers need to clone or download the Spot C++ SDK distribution from github. The distribution contains client code, programming examples, protobuf definitions and API documentation.

The Spot C++ SDK distribution is available at https://github.com/boston-dynamics/spot-cpp-sdk.

Users can either:

git clone https://github.com/boston-dynamics/spot-cpp-sdk.git (recommended)

Or download a zipfile distribution:

1. Select green box "Clone or download" from the webpage.
2. Select "Download ZIP."
3. Unzip the file to your home directory.
4. Rename the top-level directory `spot-sdk-cpp-master` to `spot-cpp-sdk`. (Not required: only for consistency with this document.)

### Compile and install the C++ SDK

To install the C++ SDK in a folder <SDK_install_path>, follow the steps below. Replace <vcpkg_install_path> with the folder where `vcpkg` is installed and <SDK_clone_path> with the folder where the SDK is cloned.

```
cd <SDK_clone_path>
cd cpp
mkdir build
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE=<vcpkg_install_path>/scripts/buildsystems/vcpkg.cmake -DCMAKE_INSTALL_PREFIX=<SDK_install_path> -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=TRUE
```

The `cmake` commands need the flag `-DCMAKE_FIND_PACKAGE_PREFER_CONFIG=TRUE`, which instructs cmake to use its own version of find package instructions, instead of the one provided by the dependency. This is necessary for the protobuf dependency, which generates three versions of the find package instructions:

- The cmake version, which we need to use
- The Protobuf version, which is included in the Protobuf vcpkg installation
- The gRPC version, which is included in the gRPC vcpkg installation, and it might be identical to the version above.

The `cmake` command can also overwrite the following variables by passing them to the `cmake` command line with a `-D` prefix:

- `CMAKE_BUILD_TYPE`: String variable to determine build type. The default value for this variable is "Release".
- `BUILD_SHARED_LIBS`: Boolean ON/OFF variable to turn on the build of shared libraries. The default value for this variable is ON. **Shared libraries currently are only supported on Linux.** This limitation is temporary and will be addressed in future releases.
- `BUILD_CHOREOGRAPHY_LIBS`: Boolean ON/OFF variable to turn on the build of choreography libraries. The default value for this variable is ON.

**On Linux:**

Build the SDK using the `make` command below.

```
make -j6 install
```

The `make` command generates a lot of deprecation warnings during the compiling of the classes generated from the protobuf definitions. This is expected as the protobuf definitions contain `deprecated` flags for fields that will not be supported in future versions of the SDK.

**On Windows:**

- Open the `sln` file in the build folder in Microsoft Visual Studio.
- Remove `libprotobuf.dll` library dependencies in the targets example executables `hello_spot`, `basic_robot_command`, `get_image` and `spot_cam`. Go to the properties of each target, then select `Configuration Properties`, then `Linker`, then `Input` and then edit the `Additional Dependencies` and remove the `libprotobuf.dll` entry from the list.
- Right click on INSTALL target and select `Build`.

## Verify you can command and query Spot

To verify your packages work correctly with Spot, you need:

- A Spot robot on the same version as your packages
- A user account on the robot

### Get a Spot robot

Contact sales@bostondynamics.com to get a Spot robot.

### Get a user account on the robot

If you have just unboxed your Spot robot, you will find a sticker inside the battery cavity with wifi, admin, and username "user" credentials. Please note however that Boston Dynamics recommends that you first have your designated robot administrator log onto the robot with admin credentials and change passwords to increase security.

NOTE: The following examples assume username "user" and password "password."

After creating the login credentials, store them in environment variables `BOSDYN_CLIENT_USERNAME` and `BOSDYN_CLIENT_PASSWORD`. You can also pass the credentials to the SDK example applications using the command-line arguments `--username` and `--password`.

### Ping Spot

1. Power on Spot by holding the power button down until the fans start. Wait for the fans to turn off (10-20 seconds).
2. Connect to Spot via wifi.
3. Ping spot at 192.168.80.3.

```shell
$ ping 192.168.80.3
```

### Run hello_spot example

Issue the following command to run the hello_spot example from the SDK install `bin/` folder:

```shell
# with `BOSDYN_CLIENT_USERNAME` and `BOSDYN_CLIENT_PASSWORD` environment variables set:
$ ./hello_spot 192.168.80.3
# without `BOSDYN_CLIENT_USERNAME` and `BOSDYN_CLIENT_PASSWORD` environment variables set:
$ ./hello_spot 192.168.80.3 --username {USER} --password {PASSWORD}
```

If this worked for you, SUCCESS! You are now successfully communicating with Spot via C++! Note that the output returned shows your Spot robot's unique serial number, its nickname and robot type (Boston Dynamics has multiple robots), software version, and install date.

## Next steps

1.  Read our next section, [Spot Programming](https://dev.bostondynamics.com/docs/python/understanding_spot_programming). Highly recommended!
2.  Take time to explore the [programming examples](../../cpp/examples/README.md) which all launch in essentially the same manner as hello_spot.

    - Try making simple modifications to the code.
    - NOTE: If you installed the SDK using a zipfile, be careful to understand what changes you've made, as users sometimes inject errors into the SDK code unintentionally. Git users can simply use `git diff` to understand all changes they have made.

3.  Read through the [protocol buffer definitions](../protos/README.md) and the Spot C++ SDK [source code documentation](../../cpp/README.md) to understand even more.

If you have any questions, please [reach out to Support](https://support.bostondynamics.com/s/contactsupport).
