FROM debian as build
RUN apt update
RUN apt install -y qt5-qmake qt5-default build-essential
COPY . .
RUN qmake
RUN make

FROM debian
RUN apt update
RUN apt install -y qt5-default
COPY --from=build /RoomServer /usr/bin
EXPOSE 1998
CMD RoomServer
