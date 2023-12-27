# The project

This is a grass renderer built using OpenGL and GLFW. The idea is to recreate the follow the [GDC talk](https://www.youtube.com/watch?v=Ibe1JBF5i5Y) of Ghost of Tsushima programmers to create beliveable grass.

## Dependcies

GLFW
OpenGL (Version > 4.5 for compute shader and DSA)
Glad

# Steps

## Step 1 - Compute shader

In this [shader](shader/grassCompute.glsl) we create each blade of grass (position, height, width, etc, ...)

For example, we can see that we have created 1024 blades of grass that are all represented by a point of random color for now (TODO: add image).

## Step 2 - Geometry shader

From the output of the compute shader, we want to create triangles that will look like grass (TODO: add image).

## Step 3 - Grass pattern

Now, instead of having regular grass pattern, we want to add some noise (TODO: add image).