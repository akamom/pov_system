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

those are the available console commands while in the pov enviroment

    add: adds image to selection
        mandatory: <file=[filename]>: file in current path 
        optional: <index=[index]>: at which index to insert
    goto: sets the current working path
        optional: <path=[path]>: path as value
    help: lists all commands
    remove: removes image from selection either by index or by name
    status: lists all selected images and index

to use the commands, you have to add the prefix command `pov` + `<command>`. Commands can also have optional or mandatory parameters.
This is an example of how to use the commands and parameters:

Let's say I would like to navigate to a directory. I can do it like this:
    
    pov goto path="C:\some path name"

the parameter path will hold have the value of the path. We will now navigate to this path and from there we can for example add images to our selection.
#### Notice the quotation marks around the path. They have to be used with parameter values which contain whitespaces!

## Debugging with VS Code

For quick debugging, please use the VS Compiler.
Further information can be found on the [CMake Tools Documentation Page](https://vector-of-bool.github.io/docs/vscode-cmake-tools/debugging.html).
