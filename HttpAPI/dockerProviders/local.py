import docker
from databaseClasses import Room, Player
from os import environ

dockercli = docker.from_env()

def startRoomContainer(room: Room, owner: Player):
	local_ip='127.0.0.1' if not environ.get('LOCAL_IP') else environ.get('LOCAL_IP')
	httpapiurl = "virtualconcerthall.urown.cloud" if not environ.get('HTTPAPIURL') else environ.get('HTTPAPIURL')
	containerEnv={
			"TOKEN":room.token.id,
			"OWNER":owner.secretId,
			"HTTPAPIURL":httpapiurl,
	}

	room=dockercli.containers.run('virtualconcerthall.azurecr.io/roomserver',ports={'1998/udp':None},environment=containerEnv,remove=True,detach=True)
	room.reload()
	ip=local_ip
	port=room.ports['1998/udp'][0]['HostPort']

	return {'ip':ip,'port':port,'id':room.id}
