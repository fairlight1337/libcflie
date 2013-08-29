Crazyflie Nano C++ Client Library
=================================

First of all, the Crazyflie Nano is an awesome Quadcopter by the guys
at [Bitcraze](http://www.bitcraze.se/). I just love it - and since I'm
a C/C++ programmer deep in my heart, I really, really wanted to have a
client library in that language (there was none at that time).

That's why I wrote one.

My work is based on Bitcraze's Python Library (see their repos) which
(alongside their wiki) greatly helped me understand the communication
protocol.  Feel free to use the library as you like, as long as you
obey the rules of the BSD license.

You can check out the current Doxygen source code documentation
[here](http://fairlight1337.github.io/libcflie/doxygen/html/).


### What this library is

* A USB connection point to the Crazyflie Nano quadcopter by Bitcraze
* An abstraction layer for the different messages you can send your copter
* A controller framework for controlling the Crazyflie using cartesian coordinates
* A C++ entrypoint for programmers that write programs in C++ (rather than Python)
* Developed, written and maintained completely by me (Jan Winkler)


### What this library is not

* A bootloader
* Complete
* Bug-free (although I'm trying really hard!)


Dependencies
------------

The lib depends on GLFW, which you can install under Ubuntu by `apt-get install`'ing this:
```
sudo apt-get install libglfw-dev libglfw2 cmake
```


How to build the library
------------------------

First, clone the repository:
```
$ git clone https://github.com/fairlight1337/libcflie.git
```

In the cloned folder, do:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

After that, you will find two directories in the base directory: `lib`
and `bin`.

* `lib` includes the built `libcflie` library (with fitting extension,
  depending on where you build it). You can link against this library,
  using the header files contained in `include/cflie/` to actually use
  it.

* `bin` includes example programs. Currently, there are two:
  * `ex-simple` shows the most simple usage example of the library
  * `ex-replugging` shows how to use the lib for allowing re-plugging
    the USB dongle and letting the copter go out of range and
    return. Also, event hooks are marked for all these events to
    execute custom code for all situations.
  * `ex-gui` displays an OpenGL window that shows a plane based on the
    current copter orientation. Using the arrow keys, you can control
    the copter, and when pressing the space bar, you can increase the
    thrust to the copter. CAREFUL: When starting the program, the
    copter will start with a thrust greater than 10000, which will
    start the engines! So be sure to place it far away from objects
    (or your hands).


How to run the examples (or you own programs)
---------------------------------------------

When running programs linked against and using libcflie, you have to
have access permissions for your USB devices set up correctly.

You basically have two options:

1. Run your programs as root. For the `ex-gui` example, this would mean (starting from the libcflie/build directory after typing `make`):
```
sudo ./../bin/ex-gui
```
Exit the example by pressing `ESC`.

2. Set up your USB permissions for the CrazyRadio dongle (this is the recommended way). For this to work, create a new file at `/etc/udev/rules.d/99-crazyradio.rules` and put this line into it:
```
SUBSYSTEM=="usb", ATTRS{idVendor}=="1915", ATTRS{idProduct}=="7777", MODE=="0664", GROUP=="plugdev"
```
Now replug your dongle and you're set. Start it with:
```
./../bin/ex-gui
```
Again, exit the example by pressing `ESC`.
