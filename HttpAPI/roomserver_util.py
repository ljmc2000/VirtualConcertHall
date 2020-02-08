import socket

def testRoom(room):
	ip=room.server.ip, room.server.port

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.connect(ip)
	sock.settimeout(2)

	try:
		sock.sendto(int.to_bytes(256-2,1,'little'),ip)
		data,addr=sock.recvfrom(1)
		n=int.from_bytes(data,'little')

		r=(n==(256-2))

	except (socket.timeout,ConnectionRefusedError) as e:
		r=False

	sock.close()
	return r
