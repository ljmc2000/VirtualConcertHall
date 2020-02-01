export MXE_PREFIX=$HOME/.local/share/mxe/usr/x86_64-w64-mingw32.shared
export BUILDDIR=/tmp/RoomClientWindows
export WINEDEBUG=+loaddll
export LIBSREQ=$(wine Client.exe 2>&1 >/dev/null | grep -Po '....:err:module:import_dll Library \K(.*.dll)' | awk '{print $1;}')
echo $LIBSREQ

mkdir $BUILDDIR
for LIB in $LIBSREQ
do
	location=$(find $PWD $MXE_PREFIX -name $LIB)
	cp $location $BUILDDIR
	echo $location
done

cp Client.exe *.svg $BUILDDIR
zip -r RoomClientWindows.zip $BUILDDIR
rm -r $BUILDDIR
