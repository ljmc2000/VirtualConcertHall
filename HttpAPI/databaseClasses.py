import bcrypt,secrets,datetime,re
from os import environ
from time import sleep
from random import getrandbits
from mongoengine import *
from mongoengine.fields import *
from exceptions import ExpiredLoginToken, ShortPassword, BadPassword, Imposter

passwordSize=8
connect('virtualconcerthall',host=environ.get('MONGO_URL'),serverSelectionTimeoutMS=100)

ROOMCLOSESTATES = ('CRASH','TIMEOUT','USER')

class WithPassword:
	def setpwd(self,password: str):
		if len(password) < passwordSize:
			raise ShortPassword(len(password))

		if not re.search('[a-z]',password):
			raise BadPassword('no lowercase')
		if not re.search('[A-Z]',password):
			raise BadPassword('no uppercase')
		if not re.search('[\d]',password):
			 raise BadPassword('no digit')
		if not re.search('[^\w]',password):
			 raise BadPassword('no symbol')
		if not re.search('^[^A-Z]',password):
			raise BadPassword('must not begin with a capital letter')
		if not re.search('[a-zA-Z]$',password):
			raise BadPassword('must not end with a digit or symbol')

		self.passhash=bcrypt.hashpw(password.encode(),bcrypt.gensalt()).decode()

	def checkpwd(self, password: str):
		return bcrypt.checkpw(password.encode(), self.passhash.encode())

class User(Document,WithPassword):
	username = StringField(unique=True,required=True,min_length=4)
	passhash = StringField(max_length=60,required=True)
	lastPing = DateTimeField()	

class LoginToken(Document):
	token=StringField(default=lambda: secrets.token_urlsafe(32),primary_key=True)
	user=ReferenceField(User, required=False)
	created = DateTimeField(default=datetime.datetime.now)
	expires = DateTimeField(default=lambda: datetime.datetime.now() + datetime.timedelta(days=365))

	def setExpiry(self,**expiry):
		self.expires = datetime.datetime.now()+datetime.timedelta(**expiry)
	def expired(self):
		return self.expires < datetime.datetime.now()

class RoomServer(Document,WithPassword):
	containerName=StringField(primary_key=True)
	ip = StringField()
	port = IntField()
	token = ReferenceField(LoginToken,required=True)
	roomCount = IntField(default=0)

def getLeastLoadedServer():
	server=RoomServer.objects.order_by("roomCount")[0]
	server.roomCount += 1

	return server

class Room(Document):
	roomname = StringField(required=True)
	roomId = LongField(primary_key=True, default=lambda: int(secrets.token_hex(8),16)-(2**63))
	server = ReferenceField(RoomServer, default=getLeastLoadedServer, required=True, reverse_delete_rule=CASCADE)
	port = IntField()
	owner = ReferenceField(User, required=True, unique=True)
	players = ListField(ReferenceField(User))
	passhash = StringField(max_length=60)
	description = StringField()
	private = BooleanField(default=False)
	created = DateTimeField(default=datetime.datetime.now)

	def close(self,reason):
		closedRoom=ClosedRoom()
		closedRoom.fromRoom(self)
		closedRoom.closureReason=reason
		closedRoom.save()

		self.server.roomCount-=1
		self.server.save()
		self.delete()

class Player(Document):
	user = ReferenceField(User, required=True, unique=True)
	secretId = IntField(default=lambda: getrandbits(32), unique=True)
	clientId = IntField(default=lambda: getrandbits(32), unique=True)
	room = ReferenceField(Room, required=True, reverse_delete_rule=CASCADE)

class ClosedRoom(Document):
	roomname = StringField(required=True)
	owner = ReferenceField(User, required=True)
	description = StringField()
	private = BooleanField(default=False)
	players = ListField(ReferenceField(User))
	created = DateTimeField(required=True)
	closed = DateTimeField(default=datetime.datetime.now)
	closureReason = StringField(required=True,choices=ROOMCLOSESTATES)

	def fromRoom(self,room: Room):
		self.roomname = room.roomname
		self.owner = room.owner
		self.description = room.description
		self.private = room.private
		self.players = room.players
		self.created = room.created

class UnexpectedError(Document):
	type=StringField(required=True)
	message=StringField(required=True)
	time=DateTimeField(default=datetime.datetime.now)
	endpoint=StringField(required=True)

def getUserByToken(token: str):
	token=LoginToken.objects.get(token=token)
	sleep(.25)       #make things harder on bad actors

	if not token.expired():
		return token.user
	else:
		token.delete()
		raise ExpiredLoginToken(token.user,token.expires)

def checkIfServerToken(token: str):
	u=getUserByToken(token)
	if u == None:
		return True
	else:
		raise Imposter(u)
