FROM ubuntu AS build
RUN apt update
RUN apt install -y wget unzip build-essential libssl-dev libnss3-dev
COPY prebuild.sh ./
RUN ./prebuild.sh
COPY *.cpp build.sh ./
RUN ./build.sh

FROM ubuntu
#RUN apk add --no-cache libstdc++
COPY --from=build roomServer /usr/bin/roomServer
CMD roomServer
