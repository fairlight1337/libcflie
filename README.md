Crazyflie Nano C++ Client Library
=================================

First of all, the Crazyflie Nano is an awesome Quadcopter by the guys at [Bitcraze](http://www.bitcraze.se/). I just love it - and since I'm a C/C++ programmer deep in my heart, I really, really wanted to have a client library in that language.

That's why I wrote one.

My work is based in Bitcraze's Python Library (see their repos) which (alongside their wiki) greatly helped me understand the communication protocol.
Feel free to use the library as you like, as long as you obey the rules of the BSD license.


### What this library is

* A USB connection point to the Crazyflie Nano quadcopter by Bitcraze
* An abstraction layer for the different messages you can send your copter
* A controller framework for controlling the Crazyflie using cartesian coordinates
* A C++ entrypoint for programmers that write programs in C++ (rather than Python)
* Developed, written and maintained completely by me (Jan Winkler)


### What this library is not

* A bootloader
* Complete


How to build the library
------------------------

First, clone the repository:
`
$ git clone https://github.com/fairlight1337/libcflie.git libcflie
`

In the cloned folder, do:
```
$ mkdir build

$ cd build

$ cmake ..

$ make
```

After that, you will find two directories in the base directory: `lib` and `bin`. `lib` includes the built `libcflie` library (with fitting extension, depending on where you build it). `bin` includes a `test` program which is defined by the content of `src/main.cpp` that shows the basic usage of the library.
