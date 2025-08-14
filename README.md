# RP2350Touch2-8C-Exp
Waveshare RP2350Touch2-8C Experiments. To support videos on my [YouTube channel](https://youtube.com/@drjonea).

A [LVGL]()https://lvgl.io) project to run on the [Waveshare RP2350-Touch-LCD-2.8C](https://www.waveshare.com/wiki/RP2350-Touch-LCD-2.8C).

## Libraries
-bsp library is take directly from the Waveshare demo on the [Wiki site](https://www.waveshare.com/wiki/RP2350-Touch-LCD-2.8C).
-lvgl is take from the master GitHub repo but checkout to version 8.4.0

## Experiments
The folder exp/drjonea contains a simple example project to draw my social media name and have
moving arcs around it. The structure of this project is different from the Waveshare examples.

This board needs a specific board definition file and at time of writing this is not part of the Pick SDK. I have therefore included a boards folder which includes the specific header file.

The LVGL configuration and port files to run LVGL on this board are included in the port folder.

BSP and LVGL are build as static libraries and linked to the binary.

## Clone and Build
I have used submodules for LVGL so please clone with the --recurse-submodules switch.

Should build with the normal toolchain commands. I do this on command line using the commands
```
mkdir build
cd build
cmake ..
make
```
 


