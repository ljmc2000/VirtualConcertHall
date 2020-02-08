if [ -z $IMAGE_NAME ]
then
	IMAGE_NAME=virtualconcerthall.azurecr.io/httpapi
fi
docker build -t $IMAGE_NAME .
docker push $IMAGE_NAME
