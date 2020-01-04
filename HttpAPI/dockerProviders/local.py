import docker
dockercli = docker.from_env()

def startRoomContainer(token: str):
	dockercli.images.pull('thegingernut/roomserver')
	room=dockercli.containers.run('thegingernut/roomserver',ports={'1998/udp':None},environment={"TOKEN":token},remove=True,detach=True)
	room.reload()
	ip='127.0.0.1'
	port=room.ports['1998/udp'][0]['HostPort']

	return {'ip':ip,'port':port,'id':room.id}

def deleteRoomContainer(id: str):
	room=dockercli.containers.get(id)
	room.stop(timeout=5)
	return True
