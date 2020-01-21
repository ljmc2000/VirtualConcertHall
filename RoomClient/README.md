# Building Instructions

* Clone submodules with git submodule init and build using the script buildDeps.sh
* For linux install the distro provided versions of Qt5 and fluidsynth
* For windows download mxe to .local/share/mxe: then run 'MXE_TARGETS=x86_64-w64-mingw32.static' make qt5; make fluidsynth
