import secrets
from os import environ
from databaseClasses import Room, Player
from azure.common.client_factory import get_client_from_cli_profile as get_client
from azure.mgmt.containerinstance import ContainerInstanceManagementClient
from azure.mgmt.containerinstance.models import *

azure_cli=get_client(ContainerInstanceManagementClient)

RESOURCE_GROUP="VirtualConcertHall" if not environ.get('RESOURCE_GROUP') else environ.get('RESOURCE_GROUP')
HTTPAPIURL="virtualconcerthall.urown.cloud" if not environ.get('HTTPAPIURL') else environ.get('HTTPAPIURL')
LOCATION="uksouth" if not environ.get('LOCATION') else environ.get('LOCATION')
DOCKER_IMAGE="virtualconcerthall.azurecr.io/roomserver" if not environ.get('DOCKER_IMAGE') else environ.get('DOCKER_IMAGE')
DOCKER_REGISTRY_URL="virtualconcerthall.azurecr.io" if not environ.get('DOCKER_REGISTRY_URL') else environ.get('DOCKER_REGISTRY_URL')
DOCKER_REGISTRY_USERNAME="virtualconcerthall" if not environ.get('DOCKER_REGISTRY_USERNAME') else environ.get('DOCKER_REGISTRY_USERNAME')
#SUBSCRIPTION_ID=environ['SUBSCRIPTION_ID']

resource_requirements=ResourceRequirements(requests=ResourceRequests(memory_in_gb=1.5, cpu=1, gpu=None))
container_ports=[ContainerPort(port=1998, protocol=ContainerNetworkProtocol.udp)]
registry_credentials=ImageRegistryCredential(
	server=DOCKER_REGISTRY_URL,
	username=DOCKER_REGISTRY_USERNAME,
	password=environ['DOCKER_REGISTRY_PASSWORD'],
)

def startRoomContainer(room: Room, owner: Player):

	container_name="roomserver-"+secrets.token_hex(10)

	container_env=[
		EnvironmentVariable(name="TOKEN",value=room.token.id),
		EnvironmentVariable(name="OWNER",value=owner.secretId),
		EnvironmentVariable(name="HTTPAPIURL",value=HTTPAPIURL),
	]

	container_properties=Container(
		name=container_name,
		image=DOCKER_IMAGE,
		ports=container_ports,
		resources=resource_requirements,
		environment_variables=container_env,
	)

	container_group_properties=ContainerGroup(
		containers=[container_properties],
		image_registry_credentials=[registry_credentials],
		os_type='Linux',
		location=LOCATION,
	)

	container_request=azure_cli.container_groups.create_or_update(
		resource_group_name=RESOURCE_GROUP,
		container_group_name=container_name,
		container_group=container_group_properties,
	)

	return {'ip':None,'port':1998,'id':container_name}
