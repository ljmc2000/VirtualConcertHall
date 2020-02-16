import re, subprocess
from os import environ

MXE_PREFIX=environ['HOME']+'/.local/share/mxe/usr/x86_64-w64-mingw32.shared'
OBJDUMP=environ['HOME']+'/.local/share/mxe/usr/bin/x86_64-w64-mingw32.shared-objdump'
BUILD_DIR='/tmp/RoomClientWindows'
build_dir='RoomClientWindows'

dllPattern=re.compile('DLL Name: (.*)')

def findDll(name):
	try:
		p=subprocess.run(('find', MXE_PREFIX, environ['PWD'], '-name', name), stdout=subprocess.PIPE, check=True)
		return p.stdout.decode().strip()
	except subprocess.CalledProcessError:
		return None

def inspectFile(name,found=[],locations=[]):
	p=subprocess.run((OBJDUMP, '-p', name), stdout=subprocess.PIPE)
	for result in dllPattern.finditer(p.stdout.decode()):
		name=result.group(1)
		if name not in found:
			found.append(name)
			f=findDll(name)
			if f:
				locations.append(f)
				inspectFile(f,found,locations)
	return locations

subprocess.run(('mkdir',BUILD_DIR))

for dependency in inspectFile('Client.exe'):
	subprocess.run(('cp',dependency,BUILD_DIR))

subprocess.run('cp Client.exe *.svg %s' % BUILD_DIR, shell=True)
subprocess.run(('cp', '-r', MXE_PREFIX+'/qt5/plugins/platforms', BUILD_DIR))
subprocess.run(('zip', '-r', 'RoomClientWindows.zip', '.'), cwd=BUILD_DIR)
subprocess.run(('rm','-r',BUILD_DIR))
