MXE_PREFIX=$HOME/.local/share/mxe/usr/x86_64-w64-mingw32.shared
PATH=$PATH:$HOME/.local/share/mxe/usr/bin/:$MXE_PREFIX/bin/
$MXE_PREFIX/qt5/bin/qmake
make

WINEPATH=$(winepath -w $MXE_PREFIX/qt5/bin/)
WINEPATH=$WINEPATH\;$(winepath -w $MXE_PREFIX/bin/)
WINEPATH=$WINEPATH\;$(winepath -w $MXE_PREFIX/lib/)
WINEPATH=$WINEPATH\;$(winepath -w $PWD/rtmidiWindows)
export WINEPATH

wine release/Client.exe

