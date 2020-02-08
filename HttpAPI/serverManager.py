import docker
from os import environ
from sys import argv
from databaseClasses import RoomServer,LoginToken

dockercli = docker.from_env()

HTTPAPIURL = "virtualconcerthall.urown.cloud" if not environ.get('HTTPAPIURL') else environ.get('HTTPAPIURL')

def create():
	token=LoginToken()
	token.save()
	roomserver=RoomServer(token=token)

	containerEnv={
		"TOKEN":token.id,
		"HTTPAPIURL":HTTPAPIURL,
	}

	ports={'1998/udp':1998}
	for i in range(10000,10100):
		ports['%d/udp'%i]=i

	room=dockercli.containers.run('virtualconcerthall.azurecr.io/roomserver',name="room_server",ports=ports,environment=containerEnv,remove=True,detach=True)
	roomserver.id=room.id
	roomserver.save()

def destroyAll():
	for room in RoomServer.objects:
		container=dockercli.containers.get(room.id)
		container.stop()
		room.delete()

functions={
	"create":create,
	"destroyAll":destroyAll,
}

if __name__ == "__main__":
	try:
		functions[argv[1]]()
	except KeyError as e:
		print("no functionality for",e)
