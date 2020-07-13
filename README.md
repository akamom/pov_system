# POV System

The script enables the user to show a pixel-art picture in an pov_system.
Therefor a PNG file is loaded into the script, transfered to the microcontroler and loaded into the pov_system.

## Installation

[CMake](https://cmake.org/) has to be installed on the local machine.

1. Clone the project into a local directory.
2. In your terminal navigate to the project folder.
3. Create the directory `build/` in the project folder and navigate to it.
4. Configurate the project `cmake ..`
5. Build the project with `make`.
6. Run project with `./pov_system`. 

## Developing with VS Code
See the [this videos](https://www.youtube.com/watch?v=_yFPO1ofyF0&list=PLK6MXr8gasrGmIiSuVQXpfFuE1uPT615s) for working with VS Code and C++ with CMake.

I also added the .vscode folder for the VS Code Settings. Be aware that those settings can vary on different machines. Maybe put it in .gitignore later.

## Console UI
Root: pov

    pov add <filename>  <optional: order> adds filename
    pov remove <filename>
    pov list: lists all files and it's index (easier to use index instead of name)
    pov dir <optional:  -set  <path>>: gets or sets standard path
    pov sort: sorts files?
    pov up: uploads files (rewrites files on raspi)
    pov start: starts pov
    pov stop

Alpha = 0: RGB: 0 (also schwarzer Pixel (alles über A>0: originale RGB Farbe))