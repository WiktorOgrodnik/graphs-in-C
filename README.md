# Graphs in C

## App description

![App screenshot](./docs/img3.png)

This app, written in C and GTK 4 is used to draw graphs of one variable in a given range. The app implements simple rasterization and microsampling thus graphs looks clear. This app was tested on Windows 10/11 and Linux.

This repo contains only the display engine. The parser and evaluator was moved to [different repo](https://github.com/WiktorOgrodnik/graphs-in-C-core).

## How to install

### Windows

1. download and install [MSYS2](https://www.msys2.org/),
2. download and install [GTK](https://www.gtk.org/docs/installations/windows/) (GTK 4.0),
3. you will also need programs such as: gcc, make, pkg-config, git, bash and so on,
4. in bash type: 'bash install.sh', you will need an internet connection,
5. run program with './graphs.exe'.

### Linux

1. download and install GTK4 for your distribution
2. download and install gcc, make, pkg-config, through your package manager
3. in bash type: 'bash install.sh', you will need an internet connection,
4. run program with './graphs'.

## How to write equations?

In the program, you can enter 4 equations at once, which will be displayed on a graph. The graphs have different colors, blue, red, green and purple, respectively.

Instructions were moved to [another repo](https://github.com/WiktorOgrodnik/graphs-in-C-core).

## Settings

In the app, there are multiple settings to manage how the graphs should be printed on the screen.

### Interval

Text field 'interval' accepts one positive number. The program will draw a graph on interval form -number to +number.

### Scale

The scale text field accepts one number (it can be an integer or a number with a finite decimal expansion). The scale is responsible for the counter-domain on which the function will be drawn. With a scale of 1.0, the smallest value you can see is f(x) = -300 and the largest is +300, with a scale of 2.0 the interval is (-150, 150), with the default scale of 30.0 the interval is (-10, 10).

### Rasterization and Microsampling

Rasterization is responsible for connecting the dots on the graph, so that the displayed graph is a continuous line. This option is recommended when the graph is rapidly increasing or decreasing, without rasterization it would be difficult to observe the course of the function.
Note that rasterization at discontinuous points may behave unpredictably, for example, for the expression 'floor(x)' the functionality will draw vertical lines at points f(k), where k is an integer, which should not happen.
Rasterization has little effect on performance and is enabled by default.

Microsampling only works when rasterization is enabled. The functionality performs additional tests at discontinuity points and decides whether to draw a line connecting the dots or not. The functionality should better reflect the graphs of complex functions.
Microsampling has a big impact on performance and is disabled by default.

## Code modules

The program includes the following modules:

- eval.h, eval.c, parse.h, parse.c -> interpreter stubs
- draw.h and draw.c -> display module
- main.c -> interface module

## Roadmap

- Add CSS to exisitning GTK 4 controls
- Saving graph to bitmap file
- Terminal parameters
- Setting menu and "What's new" window
- Adding custom math constants
- Automatic updates

## Licensing

The app is available under [MIT License](./LICENSE).
