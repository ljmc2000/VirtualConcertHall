FROM debian as build
RUN apt update
RUN apt install -y qt5-qmake qt5-default build-essential
COPY . .
RUN qmake
RUN make
RUN strip --remove-section=.note.ABI-tag /usr/lib/x86_64-linux-gnu/libQt5Core.so.5

FROM debian
RUN apt update
RUN apt install -y qt5-default
COPY --from=build /RoomServer /usr/bin
COPY --from=build /usr/lib/x86_64-linux-gnu/libQt5Core.so.5 /usr/lib/x86_64-linux-gnu
EXPOSE 1998/udp
CMD RoomServer
