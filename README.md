# Final-Project-czion-meschlem
Welcome to the SET! In this exciting C++ endeavor, we bring the engaging SET card game to life. 
Whether you're a seasoned SET strategist or a newcomer to the game, our implementation promises a captivating experience. Read on to discover the world of patterns, graphics, and fun!


## Contributors
- Cooper Z.
- Mason E.


## Summary
This C++ project implements the SET card game, a pattern recognition card game where players find sets of cards with matching or distinct features.

## Program Overview
The program utilizes the following concepts:
- Graphics
- Texturing
- Shaders
- A bunch of other Module 4 stuff

## Operating System Compatibility
The project can theoretically run on any OS, but has only been tested on Windows.

## Installations
No additional installations should be required to run the program.

## Citations
- Much of the base for this code came from the OpenGL starting code from module 4 projects. 
- Several of the files in the project were written from scratch by us.
- The image loader, which takes a png and loads it into a 2D array for OpenGL to texture with comes from [here](https://github.com/nothings/stb/blob/master/stb_image.h).
- The vertex and fragment shaders for the cards come from [here](https://learnopengl.com/Getting-started/Textures), where I learned the basics of texturing with OpenGL, though I don't believe I copied any other code.
- I created the images for the cards myself in Adobe Illustrator, and combined them all into the atlas png. 

## Points Justification
We believe the project deserves 175 points because the entirety of the code to make the game work in OpenGL exists. The logic, the custom classes, the framework, all exists. 
Additionally, all the code for the texturing is not only written, but implemented, and can be seen in the custom tex.cpp and tex.h files, which I wrote from scratch to extend shape. similarly to rect.
Despite the texturing not being functional, I believe it's very close to being functional, with some troubleshooting still needed, but not much writing of code.
I believe all of these reasons combined justifies the loss of 25 points, because, as I said, about 95% of the project is polished and finished, it just can't be seen outside the source code. 
I intend to continue working on this project, it **will** work. 
