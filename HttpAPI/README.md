# Dependencies
* https://flask.palletsprojects.com/en/1.1.x/ -- web framework
* http://mongoengine.org/ -- database connector
* https://en.wikipedia.org/wiki/Bcrypt -- if the database gets dumped the passwords are safe
* https://pypi.org/project/azure/ -- allocate and deallocate azure resources with python scripts
* https://pypi.org/project/docker/ -- allocate and deallocate local resources with python scripts

To install run pip install --user mongoengine bcrypt flask azure-cli docker  
A dockerfile is also included to build the application for deployment to the azure cloud  

# httpapi
To run locally install the dependencies listed above and type "flask run --host ::"  
To run on azure, follow the instructions in the root readme.  

# serverMangager.py
python serverManager.py create - create a roomserver instance locally and update the database  
python serverManager.py destroyAll - remove the local roomserver instance and update the database  

environment variables:
* HTTPAPIURL: The room server docker container should be able to access the host httpapi by this
* IMAGE: The name of the docker image to deploy
* IP\_ADDRESS: The ip for the httpapi to access the room server by
* PORT: What port should the room server run on?

# serverManagerAzure.py
python serverManagerAzure.py create room\_name - create a roomserver instance on the ms azure cloud and update the database  
python serverManagerAzure.py destroy room\_name - destroy a roomserver instance on the ms cloud by name room\_name and update the database  
python serverManagerAzure.py mkuser username password - create a user with a username and password by making a call to the httpapi  

environment variables:
* CONTAINER\_NAME: Name of the container internal to the group, not thought to matter
* CPUS: Number of cpu cores azure should limit the roomserver to
* HTTPAPIURL: The room server docker container should be able to access the httpapi container by this URL
* IMAGE: The name of the image to deploy including registry
* LOCATION: The phyical location of the server on which the resource will be hosted. See azure docs for a list of valid locations
* MEMORY\_IN\_GB: Self explanitory: ram in gigabytes azure should limit the roomserver to
* REGISTRY\_URL: Login server for the registry on which the image is stored
* REGISTRY\_USERNAME: Name of the registry if it is an azure registry
* REGISTRY\_PASSWORD: Password needed to download the image from the registry
* RESOURCE\_GROUP: An azure concept, see the azure documentation.
* ROOM\_PORT: Port to host room server on
