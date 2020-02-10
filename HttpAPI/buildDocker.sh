if [[ -z $IMAGE_NAME ]]
then
	IMAGE_NAME=virtualconcerthall.azurecr.io/httpapi
fi

docker build -t $IMAGE_NAME .

if [[ -n $PUSH ]]
then
	docker push $IMAGE_NAME
fi
