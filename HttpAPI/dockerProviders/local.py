import docker
from databaseClasses import Room, Player
dockercli = docker.from_env()

def startRoomContainer(room: Room, owner: Player):
	from os import environ
	local_ip='127.0.0.1' if not environ.get('LOCAL_IP') else environ.get('LOCAL_IP')
	containerEnv={
			"TOKEN":room.token.id,
			"OWNER":owner.secretId,
	}

	dockercli.images.pull('thegingernut/roomserver')
	room=dockercli.containers.run('thegingernut/roomserver',ports={'1998/udp':None},environment=containerEnv,remove=True,detach=True)
	room.reload()
	ip=local_ip
	port=room.ports['1998/udp'][0]['HostPort']

	return {'ip':ip,'port':port,'id':room.id}

def deleteRoomContainer(id: str):
	room=dockercli.containers.get(id)
	room.stop(timeout=5)
	return True
