# The project

This is a grass renderer built using OpenGL and GLFW. The idea is to recreate the follow the [GDC talk](https://www.youtube.com/watch?v=Ibe1JBF5i5Y) of Ghost of Tsushima programmers to create beliveable grass.

## Dependcies

GLFW
OpenGL (Version > 4.5 for compute shader and DSA)
Glad

# Steps

## Step 1 - Compute shader

In this [compute shader](shader/grassCompute.glsl) we create each blade of grass (position, height, width, etc, ...)

We start by generating 1024 blades of grass that are all represented by a point.

## Step 2 - Geometry shader

From the output of the compute shader, we want to create triangles that will look like grass. To do so, we send the buffers filled in the compute shader to the graphics pipeline shaders. In the vertex shader we use the vertexID to create a vertexData from the buffers. Each of the data represent a grass blade but for now it is only a position. To make the blades visible, we use a [geometryShader](shader/geom.glsl) which will generate triangles from the vertexData. For now, all the triangles have the same LOD (a unique triangle) and all of them are located at the center of their respective tile.
![First output of the geometry shader](report/trianglesOutsideGeometry.png)

## Step 3 - Grass pattern

Now, instead of having regular grass pattern, we want to add some noise to the grass blades within their respective tile (TODO: add image).