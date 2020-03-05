import requests
from azure.common.client_factory import get_client_from_cli_profile
from azure.mgmt.containerinstance import ContainerInstanceManagementClient
from azure.mgmt.containerinstance.models import *
from os import environ
from sys import argv
from time import sleep
from databaseClasses import RoomServer,LoginToken

azure_cli=get_client_from_cli_profile(ContainerInstanceManagementClient)
CONTAINER_NAME='roomserver' if not environ.get('CONTAINER_NAME') else environ.get('CONTAINER_NAME')
CPUS=2 if not environ.get('CPUS') else environ.get('CPUS')
HTTPAPIURL = environ['HTTPAPIURL']
IMAGE = "virtualconcerthall.azurecr.io/roomserver" if not environ.get('DOCKER_IMAGE') else environ.get('DOCKER_IMAGE')
LOCATION = "uksouth" if not environ.get('LOCATION') else environ.get('LOCATION')
MEMORY_IN_GB=1.5 if not environ.get('MEMORY_IN_GCONTAINER_NAMEB') else environ.get('MEMORY_IN_GB')
REGISTRY_URL="virtualconcerthall.azurecr.io" if not environ.get('REGISTRY_URL') else environ.get('REGISTRY_URL')
REGISTRY_USERNAME="virtualconcerthall" if not environ.get('REGISTRY_USERNAME') else environ.get('REGISTRY_USERNAME')
REGISTRY_PASSWORD=environ["REGISTRY_PASSWORD"]
RESOURCE_GROUP="virtualconcerthall" if not environ.get('RESOURCE_GROUP') else environ.get('RESOURCE_GROUP')
ROOM_PORT=80 if not environ.get('ROOM_PORT') else environ.get('ROOM_PORT')

#ensure certain variables are set
environ['MONGO_URL']
if not HTTPAPIURL.startswith("http"):
	raise Exception("HTTPAPIURL must begin with http")

def create(*args):
	CONTAINER_GROUP_NAME=args[0]
	token=LoginToken()
	roomserver=RoomServer(token=token)

	containerEnv=[
		EnvironmentVariable(name='TOKEN',value=token.id),
		EnvironmentVariable(name='HTTPAPIURL',value=HTTPAPIURL),
		EnvironmentVariable(name='SERVER_PORT',value=ROOM_PORT),
	]

	ports=[
		Port(
			port=ROOM_PORT,
			protocol=ContainerGroupNetworkProtocol.udp,
		),
	]

	ipAddress=IpAddress(
		ports=ports,
		type=ContainerGroupIpAddressType.public,
		dns_name_label="%s-%s" % ("virtualconcerthall",CONTAINER_GROUP_NAME),
	)

	credential=ImageRegistryCredential(
		server=REGISTRY_URL,
		username=REGISTRY_USERNAME,
		password=REGISTRY_PASSWORD,
	)

	resourceRequests=ResourceRequests(
		memory_in_gb=MEMORY_IN_GB,
		cpu=CPUS,
	)

	container=Container(
		name=CONTAINER_NAME,
		image=IMAGE,
		resources=ResourceRequirements(requests=resourceRequests),
		ports=ports,
		environment_variables=containerEnv,
	)

	properties=ContainerGroup(
		os_type=OperatingSystemTypes.linux,
		location=LOCATION,
		image_registry_credentials=[credential],
		ip_address=ipAddress,
		containers=[container],
	)

	listener=azure_cli.container_groups.create_or_update(
		resource_group_name=RESOURCE_GROUP,
		container_group_name=CONTAINER_GROUP_NAME,
		container_group=properties,
	)

	print("allocating container on azure")
	while not listener.done():
		sleep(5)

	c=listener.result()
	print(c.provisioning_state)

	roomserver.id=CONTAINER_GROUP_NAME
	roomserver.ip=c.ip_address.fqdn
	roomserver.port=ROOM_PORT
	token.save()
	roomserver.save()

def mkuser(*args):
	username=args[0]
	password=args[1]

	r=requests.post(url=HTTPAPIURL+'/register',
			json={"username":username,"password":password})
	print(r.text)

def destroy(*args):
	CONTAINER_GROUP_NAME=args[0]
	server=RoomServer.objects.get(containerName=CONTAINER_GROUP_NAME)
	azure_cli.container_groups.delete(
		resource_group_name=RESOURCE_GROUP,
		container_group_name=CONTAINER_GROUP_NAME,
	)
	server.token.delete()
	server.delete()

functions={
	"create":create,
	"mkuser":mkuser,
	"destroy":destroy,
}

if __name__ == "__main__":
	sleep(1)
	try:
		functions[argv[1]](*argv[2:])
	except KeyError as e:
		print("no functionality for",e)
