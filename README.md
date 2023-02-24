# mhw-gui-editor
A GUI File Editor for Monster Hunter World

# Building
Install the following packages using [vcpkg](https://github.com/microsoft/vcpkg/):
```
./vcpkg install imgui[core,dx11-binding,win32-binding,docking-experimental] fmt spdlog --triplet=x64-windows-static
```

Then build the project in Visual Studio.
