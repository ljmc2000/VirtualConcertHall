def startRoomContainer(token: str):
	from os import environ
	local_ip='127.0.0.1' if not environ.get('LOCAL_IP') else environ.get('LOCAL_IP')
	print('DUMMY DRIVER: starting room')
	return {'ip':local_ip,'port':1998,'id':'DUMMYROOM'}

def deleteRoomContainer(id: str):
	print('DUMMY DRIVER: deleting room')
	return True
