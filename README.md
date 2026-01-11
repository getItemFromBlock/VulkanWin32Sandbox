# Vulkan Win32 Sandbox

### Re-learning the basics of vulkan using the windows c/c++ API

This project aims to experiments with vulkan and do weird/funny things with windows,
as using the win32 API gives us more control over our window than most multiplatform libraries.

The current main branch shows a 3D boinds simulation running entirely on the GPU, using
compute shaders and instancing. Each individual boid is a 3D model of a textured cube.
A basic lighting is applied on each face.

The `Video-Decode` branch has an in-progress video decoding feature, which is unfinished.

## Controls

Use WASD to move (or ZQSD if using an AZERTY keyboard, the controls are layout-independent).
the E and Q keys can be used to move the camera up or down. You can capture the mouse cursor by pressing escape.
When captured, the mouse cursor rotates the camera. Press escape again to free the cursor.
Use the up and down arrow keys to change the FOV. F11 Can be used to set the window in fullscreen.

## Compiling

This project is for windows 10+ only. It should work on windows 7, but I recommend using at least windows 10.

Make sure that you have the latest vulkan skd version. This project was tested and compiled against vulkan `1.4.325`.
The `VULKAN_SDK` environment variable has to be defined, and point to the vulkan installation folder.

Git clone the project, and then open the .sln file.
The Debug and Release configurations are self explanatory. The UnitTest_D/R configurations are used by the automated github actions.
You can use the UnitTest_R configuration if you want to have the logs appear in a separate terminal window, otherwise the logs all gets
redirected to the visual studio output.

## Notice about the transparent framebuffer feature

The project will try to draw in a transparent window, in such a way that the desktop appears behind it. This feature does not work
on devices which uses a different GPU to draw the desktop than the one used by the application.
If your computer has multiple GPUs, you can try switching the selected GPU using the `--device=(any number)` command line argument.
See the application logs for a listing of all detected GPUs.
