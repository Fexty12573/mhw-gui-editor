# MHW GUI Editor
An editor for the MHW GUI files (.gui)

In its current state it is not much more than a glorified 010 Editor template
with some convenience features such as a texture viewer and a proper object tree.
(There are a couple more things ofc)

![Screenshot](https://github.com/Fexty12573/mhw-gui-editor/blob/master/Screenshot%202023-08-06%20181249.png)

# Usage
### Setup
Set your chunk and nativePC directories in the settings menu (Tools > Options).
This lets the editor load textures and display them. If a texture is present in your nativePC, then
that texture will be displayed instead of the one in the chunk directory.

**Note:** "Chunk directory" refers to the directory where you *extracted* the chunks.

### To start editing
File > Open > Select a .gui file


# Building
Install the following packages using [vcpkg](https://github.com/microsoft/vcpkg/):
```
./vcpkg install imgui[core,dx11-binding,win32-binding,docking-experimental] fmt spdlog --triplet=x64-windows-static
```

Then build the project in Visual Studio.

# Todo
- [ ] Allow editing the rest of the GUI properties
- [ ] Allow inserting new objects without breaking the file
- [ ] Allow inserting new textures without breaking the file
- [ ] Add a preview window (maybe, would be cool but would need full understanding of the file)
