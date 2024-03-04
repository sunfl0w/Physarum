# Simple Physarum Simulation

A simple simulation program simulating Physarum behaviour written in C and using OpenGL compute shaders.
Inspired by and based on this [video](https://www.youtube.com/watch?v=X-iSQQgOd1A&t=917s) by Sebastian Lague.

### Command Args

-W (int) => Simulation width (also window width) in pixels. Must be a multiple of 10.

-H (int) => Simulation height (also window height) in pixels. Must be a multiple of 10.

-N (int) => Number of simulated individuals. Maximum individual count is 2097152.

-S (int) => Simulation speed.

-D (int) => Individual sample distance in pixels.

-T (int) => Individual turn rate.

-B (float) => Individual trail blur weight.

-R (float) => Individual trail decay rate.

-F => Enables FPS-Counter.

### Screenshot

![Screenshot](PhysarumSim.png)