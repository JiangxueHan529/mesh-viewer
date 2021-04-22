# mesh-viewer

OPenGL applications for viewing meshes

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake -G "Visual Studio 16 2019" ..
mesh-viewer/build $ start CS312-MeshViewer.sln
```

Your solution file should contain multiple projects, such as `mesh-viewer`.
To run from the git bash command shell, 

```
mesh-viewer/build $ ../bin/Debug/mesh-viewer
```

*macOS*

Open terminal to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ make
```

To run each program from build, you would type

```
mesh-viewer/build $ ../bin/mesh-viewer
```
Unique Feature Implemented: toon shading. 
The shaders are toon.vs and toon.fs.

I'm documenting the results by showing gifs so that the camera control can be better displayed.

1. Unlit (default) shader\
![alt text](https://github.com/JiangxueHan529/mesh-viewer/blob/main/images/unlit.gif)

2. phong shader\
![alt text](https://github.com/JiangxueHan529/mesh-viewer/blob/main/images/phong.gif)
3. toon shader\
4. ![alt text](https://github.com/JiangxueHan529/mesh-viewer/blob/main/images/toon.gif)


