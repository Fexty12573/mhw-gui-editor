# mhw-gui-editor
A GUI File Editor for Monster Hunter World

# Building
Install the following packages using [vcpkg](https://github.com/microsoft/vcpkg/):
```
./vcpkg install imgui imgui[dx11-binding] imgui[win32-binding] imgui[docking-experimental] directxtex fmt --triplet=x64-windows-static
```

Then build the project in Visual Studio.
