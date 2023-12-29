# The project

This is a grass renderer built using OpenGL and GLFW. The idea is to recreate the follow the [GDC talk](https://www.youtube.com/watch?v=Ibe1JBF5i5Y) of Ghost of Tsushima programmers to create beliveable grass.

## Dependcies

- GLFW
- OpenGL (Version > 4.5 for compute shader and DSA)
- Glad

# Steps

## Step 1 - Compute shader

In this [compute shader](shader/grassCompute.glsl) we create each blade of grass (position, height, width, etc, ...)

We start by generating 1024 blades of grass that are all represented by a point.

## Step 2 - Geometry shader

From the output of the compute shader, we want to create triangles that will look like grass. To do so, we send the buffers filled in the compute shader to the graphics pipeline shaders. In the vertex shader we use the vertexID to create a vertexData from the buffers. Each of the data represent a grass blade but for now it is only a position. To make the blades visible, we use a [geometryShader](shader/geom.glsl) which will generate triangles from the vertexData. For now, all the triangles have the same LOD (a unique triangle) and all of them are located at the center of their respective tile.
![First output of the geometry shader](report/trianglesOutsideGeometry.png)

## Step 3 - Grass pattern

Now, instead of having regular grass pattern, we want to add some noise to the grass blades. We then have to redefined our way of representing grass blades. From now on, all our grass blades will be located within the same tile (in a real application we would have multiple tile and multiple computer shaders (one per tile)).

- This tile is divided into a regular grid 
- For every intersection in that grid, we create a clump point (think of it as the center of a grass pattern, or as a voronoï vertex)
- We use a hash function to randomly jitter each of these clump points (we want a hash function and not some random noise so we don't have to store their values but just recalculate them everytime in the compute shader)
- For each of the grass blade we want to draw, we assign them a random position in 2D ![Random position](report/trianglesRandomPosition.png)
- We then find the 9 closest intersection in the grid (or less if it is at a corner, the top, the bottom, the left or the right of the grid)
- We then find the closest voronoï vertex to the grass blade (using now the jittered position of the previous 9 intersections)
- For every voronoï cells, we have defined custom information (blade's height, width, orientation, etc...) that we then use to create the vertex data of that blade that we will send to the vertex shader. In the following image, we have assigned a random color for each of the clumps (or voronoi areas) ![Clump colors](report/trianglesClumps.png)