# Building Instructions

* Clone submodules with git submodule init and build using the script buildDeps.sh
* For linux install the distro provided versions of Qt5 and fluidsynth
* For windows download mxe to $HOME/.local/share/mxe: then run 'make MXE_TARGETS=x86_64-w64-mingw32.static cmake qt5 fluidsynth'
* You can also do the same but with MXE_TARGETS=x86_64-w64-mingw32.shared to install an LGPL complient cross complier. The additional step of removing jack as a dependency of fluidsynth is required for successful installation.
