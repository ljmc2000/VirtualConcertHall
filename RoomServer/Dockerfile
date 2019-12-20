FROM vookimedlo/ubuntu-qt:latestDistroOfficial_gcc_bionic
COPY . .
RUN qmake
RUN make
EXPOSE 1998
CMD /RoomServer
