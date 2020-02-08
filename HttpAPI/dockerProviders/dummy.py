from databaseClasses import Room, Player
from os import environ

def startRoomContainer(room: Room, player: Player):
	local_ip='127.0.0.1' if not environ.get('LOCAL_IP') else environ.get('LOCAL_IP')
	print('DUMMY DRIVER: starting room')
	return {'ip':local_ip,'port':1998,'id':'DUMMYROOM'}
