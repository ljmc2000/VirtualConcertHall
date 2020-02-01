echo static or shared?; read MODE
unset CFLAGS CXXFLAGS
PATH=$PATH:$HOME/.local/share/mxe/usr/bin/:$HOME/.local/share/mxe/usr/x86_64-w64-mingw32.$MODE/bin/
mkdir rtmidiWindows rtmidiLinux

for module in 'rtmidi'
do
	cd $module'Windows'
	~/.local/share/mxe/usr/bin/x86_64-w64-mingw32.$MODE-cmake ../$module
	make
	cd ..

	cd $module'Linux'
	cmake ../$module
	make
	cd ..
done

