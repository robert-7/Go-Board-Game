# Go Video Game Project in C++

My 4th year Computer Graphics course had a few assignments. 
The third assignment asked us to develop a game using the [OpenGL graphics library](https://www.opengl.org/) and [DevIL image library](http://openil.sourceforge.net/).
The result was my group developed a 3D Go board game.
To view a working product, feel free to check out this [YouTube video](https://www.youtube.com/shorts/47JSJlxC_so).

## Setup

This project has only been tested on Ubuntu.
The instructions for setting up can be seen below.

### Setup on Windows Subsystem Linux 2 and Remote

This project assumes you're building on Windows Subsystem Linux 2.
[The instructions for installing can be found here](https://docs.microsoft.com/en-us/windows/wsl/install-win10).

#### Troubleshooting Desktop-Related Issues

Moreover, to run the binary, you might need to perform addditional setup for a working remote desktop connection.
[The instructions for setting that up can be found here](https://harshityadav95.medium.com/install-gui-desktop-in-wsl2-ubuntu-20-04-lts-in-windows-10-ae0d8d9e4459).

### Setup the Build Environment

With the above done, you should be ready to set up the needed environment.

```shell
# install dependencies
sudo apt install mesa-common-dev libglu1-mesa-dev freeglut3-dev # OpenGL libs
sudo apt install libdevil1c2 libdevil-dev                       # DevIL libs

# clone the repository
git clone https://github.com/robert-7/Go-Board-Game.git && cd Go-Board-Game
```

### Building and Running

Building requires the OpenGL and DevIL libraries imported.

* `-lstdc++` was added to resolve [this issue](https://stackoverflow.com/questions/33263288/libstdc-dso-missing-from-command-line)
* `-lm` was added to resolve [this issue](https://stackoverflow.com/questions/16006145/ld-undefined-reference-to-symbol-log2glibc-2-2-5).

The commands below should work build and run the game:

```shell
gcc -o main main.cpp -lGL -lGLU -lglut -lIL -lILU -lILUT -lstdc++ -lm
./main
```

## In-Game Commands

Below are the keyboard explanations:

```plaintext
1 : Toggle specularity
3 : Toggle wireframe
5 : Toggle depth testing
6 : Toggle culling (try disabling both depth testing and culling)
7 : Toggle between smooth and flat shading
8 : Toggle texture

o,O : Toggle teapot rotation along the y-axis
p,P : Toggle teapot rotation along the x-axis
r,R : Activate "reset board option"
y,Y : Confirm  "reset board option"
n,N : Cancel   "reset board option"
v,V : Set color to white (This makes testing a bit faster)
b,B : Set color to black (This makes testing a bit faster)

wasd  : Move a piece around the board
enter : Place a piece on the board

ON RIGHT-CLICK AND HOLD MOUSE:
* slide left  : move camera left
* slide right : move camera right
* slide up    : zoom camera in
* slide down  : zoom camera out
```
