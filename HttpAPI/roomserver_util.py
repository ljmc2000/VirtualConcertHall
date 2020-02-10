import socket

def informServerOfUpdate(roomServer):
	ip=roomServer.ip, roomServer.port

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.connect(ip)
	sock.settimeout(2)

	sock.sendto(int.to_bytes(256-3,1,'little'),ip)
	sock.close()

def testRoom(room):
	if room.server.port==None:
		return False
	ip=room.server.ip, room.server.port

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.connect(ip)
	sock.settimeout(2)

	try:
		header=(256-2)	#PING=-2 see roomcommon.h
		payload=room.id<<8
		sock.sendto(int.to_bytes(header+payload,5,'little',signed=True),ip)
		data,addr=sock.recvfrom(1)
		n=int.from_bytes(data,'little')

		r=(n==(256-2))

	except (socket.timeout,ConnectionRefusedError) as e:
		r=False

	sock.close()
	return r
