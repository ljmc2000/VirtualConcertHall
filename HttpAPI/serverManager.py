import docker
from os import environ
from sys import argv
from databaseClasses import RoomServer,LoginToken

dockercli = docker.from_env()

HTTPAPIURL = "virtualconcerthall.urown.cloud" if not environ.get('HTTPAPIURL') else environ.get('HTTPAPIURL')
IMAGE = "virtualconcerthall.azurecr.io/roomserver" if not environ.get('DOCKER_IMAGE') else environ.get('DOCKER_IMAGE')
IP_ADDRESS = environ.get('IP_ADDRESS')

def create():
	token=LoginToken()
	token.save()
	roomserver=RoomServer(token=token)

	containerEnv={
		"TOKEN":token.id,
		"HTTPAPIURL":HTTPAPIURL,
		"IP_ADDRESS":IP_ADDRESS,
	}

	ports={'1998/udp':1998}

	room=dockercli.containers.run(IMAGE,
		name="room_server",
		ports=ports,
		environment=containerEnv,
		remove=False,
		detach=True
	)
	roomserver.id=room.id
	roomserver.save()

def destroyAll():
	for room in RoomServer.objects:
		try:
			container=dockercli.containers.get(room.id)
			container.stop()
		except:
			pass
		room.token.delete()
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
