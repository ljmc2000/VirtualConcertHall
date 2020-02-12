import docker
from os import environ
from sys import argv
from databaseClasses import RoomServer,LoginToken

dockercli = docker.from_env()

HTTPAPIURL = environ['HTTPAPIURL']
IMAGE = "virtualconcerthall.azurecr.io/roomserver" if not environ.get('DOCKER_IMAGE') else environ.get('DOCKER_IMAGE')
IP_ADDRESS = environ['IP_ADDRESS']
PORT = 1998 if not environ.get('PORT') else int(environ.get('PORT'))

def create():
	token=LoginToken()
	token.save()
	roomserver=RoomServer(token=token)

	containerEnv={
		"TOKEN":token.id,
		"HTTPAPIURL":HTTPAPIURL,
	}

	ports={('%d/udp' % PORT):PORT}

	room=dockercli.containers.run(IMAGE,
		name="room_server",
		ports=ports,
		environment=containerEnv,
		remove=True,
		detach=True
	)
	roomserver.id=room.id
	roomserver.ip=IP_ADDRESS
	roomserver.port=PORT
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
