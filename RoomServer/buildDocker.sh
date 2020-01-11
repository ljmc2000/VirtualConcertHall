./clean.sh
docker build -f Dockerfile .. -t thegingernut/roomserver &&
docker push thegingernut/roomserver
