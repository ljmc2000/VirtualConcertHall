FROM alpine AS build
RUN apk add --no-cache build-base
COPY *.cpp build.sh ./
RUN ./build.sh

FROM alpine
RUN apk add --no-cache libstdc++
COPY --from=build roomServer /usr/bin/roomServer
CMD roomServer
