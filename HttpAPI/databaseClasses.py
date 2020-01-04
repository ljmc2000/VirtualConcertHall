import bcrypt,secrets,datetime,re
from os import environ
from time import sleep
from random import getrandbits
from mongoengine import *
from mongoengine.fields import *
from exceptions import ExpiredLoginToken, ShortPassword, BadPassword, Imposter

passwordSize=8
connect(environ['MONGO_URL'],serverSelectionTimeoutMS=100)

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
		if not re.search('[^\d\w]',password):
			 raise BadPassword('no symbol')

		self.passhash=bcrypt.hashpw(password.encode(),bcrypt.gensalt()).decode()

	def checkpwd(self, password: str):
		return bcrypt.checkpw(password.encode(), self.passhash.encode())

class User(Document,WithPassword):
	username = StringField(unique=True,required=True,min_length=4)
	passhash = StringField(max_length=60,required=True)

class IpAddress(EmbeddedDocument):
	ip = StringField(required=True)
	port = IntField(required=True)

class LoginToken(Document):
	token=StringField(default=lambda: secrets.token_urlsafe(32),primary_key=True)
	user=ReferenceField(User, required=False)
	created = DateTimeField(default=datetime.datetime.now)
	expires = DateTimeField(default=lambda: datetime.datetime.now() + datetime.timedelta(days=365))

	def setExpiry(self,**expiry):
		self.expires = datetime.datetime.now()+datetime.timedelta(**expiry)
	def expired(self):
		return self.expires < datetime.datetime.now()

class Room(Document,WithPassword):
	ipaddress = EmbeddedDocumentField(IpAddress,required=True)
	token = ReferenceField(LoginToken,required=True)
	containerid = StringField(required=True,unique=True)
	roomname = StringField(required=True)
	owner = ReferenceField(User, required=True, unique=True)
	players = ListField(ReferenceField(User))
	passhash = StringField(max_length=60)
	description = StringField()
	private = BooleanField(default=False)
	created = DateTimeField(default=datetime.datetime.now)

class Player(Document):
	user = ReferenceField(User, required=True, unique=True)
	secretId = IntField(default=lambda: getrandbits(32), unique=True)
	clientId = IntField(default=lambda: getrandbits(32), unique=True)
	room = ReferenceField(Room, required=True)

class ClosedRoom(Document):
	roomname = StringField(required=True)
	owner = ReferenceField(User, required=True)
	description = StringField()
	private = BooleanField(default=False)
	players = ListField(ReferenceField(User))
	created = DateTimeField(required=True)
	closed = DateTimeField(default=datetime.datetime.now)

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
