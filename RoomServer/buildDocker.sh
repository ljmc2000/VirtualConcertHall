./clean.sh
if [ -z $IMAGE_NAME ]
then
	IMAGE_NAME=virtualconcerthall.azurecr.io/roomserver
fi
docker build -f Dockerfile .. -t $IMAGE_NAME &&
docker push $IMAGE_NAME
