FROM alpine AS build
RUN apk add -v --no-cache build-base libressl-dev nss
RUN g++ --version
COPY *.cpp prebuild.sh build.sh ./
RUN ./prebuild.sh
RUN ./build.sh

FROM alpine
RUN apk add --no-cache libstdc++
COPY --from=build roomServer /usr/bin/roomServer
CMD roomServer
