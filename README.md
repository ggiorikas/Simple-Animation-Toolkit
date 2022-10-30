
# Simple Animation Toolkit

This repository hosts C++ animation primitives built on top of [raylib](https://www.raylib.com/) and demonstrates their usage through a simple example. 

## Prerequisites

In order to build and run the code, you will need a C++14 compiler and [raylib](https://github.com/raysan5/raylib) (tested with v4.2.0).

## Build Instructions

Simply compile and link to raylib, e.g. (on Linux):

```bash
$ g++ -std=c++14 -O2 -Iraylib42/include main.cpp -Lraylib42/lib -lraylib
```

Before running the executable you might also need to configure your `LD_LIBRARY_PATH` (on Linux):

```bash
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:raylib42/lib
```

You can find a video of an example run in the `output` directory.

## License

Code is licensed under MIT. Assets are licensed individually / independently -- see [Credits](#credits) for details.

## Credits

I do not own the assets included in this repository -- they are either adapted or used "as-is" from sources listed below:

- **Font** &nbsp; [Press Start](https://www.1001fonts.com/press-start-font.html) &nbsp; by codeman38
- **Music** &nbsp; [Sneaky Adventure](https://www.youtube.com/watch?v=py2IOtV7lFU) &nbsp; by Kevin MacLeod
- **SFX** &nbsp; [Cat Meowing](https://freesound.org/people/NoiseCollector/sounds/4914/) &nbsp; by NoiseCollector
- **Background** &nbsp; [Kittens](https://www.videvo.net/video/kittens-against-white-background/4775/) &nbsp; by juanjose12
- **Sprites** 
    - [Mouse](https://www.freepik.com/free-vector/design-with-seamless-pattern-gray-rat_7034013.htm) &nbsp; by brgfx
    - [Cheese](https://www.freepik.com/free-vector/english-idiom-with-be-like-chalk-cheese_24059854.htm) &nbsp; by brgfx

