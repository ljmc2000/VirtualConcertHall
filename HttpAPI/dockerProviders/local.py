import docker
dockercli = docker.from_env()

def startRoomContainer(token: str):
	from os import environ
	local_ip='127.0.0.1' if not environ.get('LOCAL_IP') else environ.get('LOCAL_IP')

	dockercli.images.pull('thegingernut/roomserver')
	room=dockercli.containers.run('thegingernut/roomserver',ports={'1998/udp':None},environment={"TOKEN":token},remove=True,detach=True)
	room.reload()
	ip=local_ip
	port=room.ports['1998/udp'][0]['HostPort']

	return {'ip':ip,'port':port,'id':room.id}

def deleteRoomContainer(id: str):
	room=dockercli.containers.get(id)
	room.stop(timeout=5)
	return True
