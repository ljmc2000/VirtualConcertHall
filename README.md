Just in case corrections are still in progress, the version of the application as it existed on the submission deadline may be found [here](https://github.com/fmtovland/VirtualConcertHall/tree/086616f5b41a545fb2acc43d95dcdf77697aa411)

# Summary

My project is a realtime networking application. 
It is the goal of my project to create a virtual space where musicians can play together in realtime. 
The users will be divided into “rooms” and users in the same room will able to hear one another as well as see what notes the other players are pressing. 
The latency is largely dependent on your internet connection. 
Users must log in in order to open a room. 
Only data that is midi encodable will be transmitable. 
Singing will be impossible. 
The client and server was developed using the Qt framework. 
The httpapi was developed using flask. 
MongoDB is used as a backend. 
Microsoft CosmosDB can be used seemlessly in it's place. 

# Deployment instructions

Local

* Run the buildDocker.sh script in RoomServer. 
* Ensure mongodb is running locally. Alternatively set the MONGO_URL environment variable.
* Change directory to HttpAPI and run flask run --host ::
* Use the serverManager.py script to create a roomserver instance
* Change directory to RoomClient. Set HTTPAPIURL to http://localhost:5000. 
  * Run ./build.sh to make a native build
  * ./buildWindows.sh will make a LGPL violating windows build.
  * ./buildLGPLCompliantWindows will make a non LGPL violating windows build.

On Azure

* Create a CosmosDB instance on azure. Make a note of your connection string.
* Create an azure container registry. Make a note of all authentication details.
* On your computer, login to your new container registry. docker login $FOO.azurecr.io, username=$FOO, password can be gotten from azure
* Change directory to HttpAPI. Run PUSH=y IMAGE_NAME=$FOO.azurecr.io/httpapi ./buildDocker
* Change directory to RoomServer. Run PUSH=y IMAGE_NAME=$FOO.azurecr.io/roomserver ./buildDocker
* Go back to azure and create a container instance. 
  * name as you please.
  * Set the image to the one on the registry you created earlier.
  * Set the DNS label to something. I normally went with virtualconcerthall
  * Create an environment variable MONGO_URL. Set it to your primary connection string from earlier
  * Once deployment completes make an note of the FQDN
* On your computer set the following variables
  * HTTPAPIURL=http://$FQDN
  * MONGO\_URL=$COSMOSDB\_CONNECTION\_STRING 
  * REGISTRY\_URL, REGISTRY\_USERNAME and REGISTRY\_PASSWORD, all based on the container registry's connection details
* Change directory to HttpAPI. use the serverManagerAzure.py script to create a server and users.
* Build a client as above. Login and have fun.
