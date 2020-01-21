unset CFLAGS CXXFLAGS
PATH=$PATH:$HOME/.local/share/mxe/usr/bin/:$HOME/.local/share/mxe/usr/x86_64-w64-mingw32.static/bin/
mkdir rtmidiWindows rtmidiLinux fluidsynthWindows fluidsynthLinux

for module in 'rtmidi' 'fluidsynth'
do
	cd $module'Windows'
	~/.local/share/mxe/usr/bin/x86_64-w64-mingw32.static-cmake ../$module
	make
	cd ..

	cd $module'Linux'
	cmake ../$module
	make
	cd ..
done

