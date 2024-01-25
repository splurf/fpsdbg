# fpsdbg

Renders a rotating cube, using OpenGL (GLSL 4.6).

<div align="center">
  <img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExNXFwMzRibHQ0NHB0c2s3NDc4MXdjOHJsZGhxanVoenVoOHd4cXEydyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/nKMxtFth5NcMsxNV5c/giphy.gif"/>
</div>

## Description
This repo will serve as a portfolio for my progression towards learning OpenGL. As of right now, it simply renders a rotating cube. Hopefully, when I have more time, I'll implement more features. The goal of this is to further my understanding of OpenGL and graphics rendering in general.

## Dependencies
+ libglfw3 (v3.3.6-*)
+ libglew-dev (v2.2.0-*)

## Build
Clone the project then go into the project directory.
```
git clone https://github.com/splurf/fpsdbg
cd fpsdbg
```
Then run `make` or `make release` to build the project.
```
make release
```

## Usage
As of right now, there are no additional flags.
```
./target/release/fpsdbg
```