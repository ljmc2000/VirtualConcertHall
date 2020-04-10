# Dependencies
* http://www.fluidsynth.org/ -- convert midi to audible tones
* https://github.com/thestk/rtmidi -- midi input and output
* https://www.qt.io/ -- build native looking user interfaces easily
* http://mxe.cc -- build on linux, run on windows

# Building Instructions

* Clone submodules with git submodule init and build using the script buildDeps.sh
* For linux install the distro provided versions of Qt5 and fluidsynth
* For windows download mxe to $HOME/.local/share/mxe:
  * If you wish to make a static build run 'make MXE_TARGETS=x86_64-w64-mingw32.static cmake qt5' then manually download fluidsynth from github and run x86\_64-w64-ming32.static-cmake .; make install; 
  * If you wish to remain LGPL complient, remove jack as a dependency of src/fluidsynth.mk and run 'MXE_TARGETS=x86_64-w64-mingw32.shared cmake qt5 fluidsynth'
* Run the script buildDeps.sh to build rtmidi for all platforms. It will ask whether you want to build the windows version as static or shared.
* run ./build.sh for a native build, ./buildWindows.sh for a windows static build or ./buildLGPLComplientWindows.sh for a correct windows build. Package with packageWindows.py
