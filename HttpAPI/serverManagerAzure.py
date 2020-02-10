from azure.common.client_factory import get_client_from_cli_profile
from azure.mgmt.containerinstance import ContainerInstanceManagementClient
from azure.mgmt.containerinstance.models import *
from os import environ
from sys import argv
from time import sleep
from databaseClasses import RoomServer,LoginToken

azure_cli=get_client_from_cli_profile(ContainerInstanceManagementClient)
HTTPAPIURL = environ['HTTPAPIURL']
IMAGE = "virtualconcerthall.azurecr.io/roomserver" if not environ.get('DOCKER_IMAGE') else environ.get('DOCKER_IMAGE')
RESOURCE_GROUP="virtualconcerthall" if not environ.get('RESOURCE_GROUP') else environ.get('RESOURCE_GROUP')
LOCATION = "uksouth" if not environ.get('LOCATION') else environ.get('LOCATION')
REGISTRY_URL="virtualconcerthall.azurecr.io" if not environ.get('REGISTRY_URL') else environ.get('REGISTRY_URL')
REGISTRY_USERNAME="virtualconcerthall" if not environ.get('REGISTRY_USERNAME') else environ.get('REGISTRY_USERNAME')
REGISTRY_PASSWORD=environ["REGISTRY_PASSWORD"]
MEMORY_IN_GB=1.5 if not environ.get('MEMORY_IN_GB') else environ.get('MEMORY_IN_GB')
CPUS=2 if not environ.get('CPUS') else environ.get('CPUS')
CONTAINER_NAME='roomserver'

#ensure certain variables are set
environ['MONGO_URL']
if not HTTPAPIURL.startswith("http"):
	raise Exception("HTTPAPIURL must begin with http")

def create(*args):
	CONTAINER_GROUP_NAME=args[0]
	token=LoginToken()
	token.save()
	roomserver=RoomServer(token=token)

	containerEnv=[
		EnvironmentVariable(name='TOKEN',value=token.id),
		EnvironmentVariable(name='HTTPAPIURL',value=HTTPAPIURL),
	]

	ports=[
		Port(port=1998,protocol=ContainerGroupNetworkProtocol.udp),
	]

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
		ip_address=IpAddress(ports=ports,type=ContainerGroupIpAddressType.public),
		containers=[container],
	)

	azure_cli.container_groups.create_or_update(
		resource_group_name=RESOURCE_GROUP,
		container_group_name=CONTAINER_GROUP_NAME,
		container_group=properties,
	)

	roomserver.id=CONTAINER_GROUP_NAME
	roomserver.save()

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
	"destroy":destroy,
}

if __name__ == "__main__":
	sleep(1)
	try:
		functions[argv[1]](*argv[2:])
	except KeyError as e:
		print("no functionality for",e)
