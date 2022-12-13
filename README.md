# Test Server and Client Assignment for UI

## Dependencies

Both server and client require Boost and CMake.

## Build

Building the server:

```
cd server

mkdir build && cd build
cmake ..

make
```

Building the client:

```
cd client
mkdir build && cd build
cmake ..

make
```

## Usage

The server accepts no arguments, listens on port 5001 and forks into background.

The client requires the server's IP address or hostname as an argument.
