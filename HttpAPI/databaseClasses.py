import bcrypt,secrets,datetime,re
from os import environ
from mongoengine import *
from mongoengine.fields import *
from exceptions import ExpiredLoginToken, ShortPassword, BadPassword

passwordSize=8
connect(environ['MONGO_URL'])

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

class Room(Document,WithPassword):
	ipaddress = EmbeddedDocumentField(IpAddress,required=True)
	containerid = StringField(required=True,unique=True)
	roomname = StringField(required=True)
	owner = ReferenceField(User, required=True, unique=True)
	passhash = StringField(max_length=60)
	description = StringField()
	private = BooleanField(default=False)
	players = ListField(ReferenceField(User))
	created = DateTimeField(default=datetime.datetime.now)

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

class LoginToken(Document):
	token=StringField(default=lambda: secrets.token_urlsafe(32),primary_key=True)
	user=ReferenceField(User, required=True)
	created = DateTimeField(default=datetime.datetime.now)

	def expired(self):
		return datetime.datetime.now() > (self.created+datetime.timedelta(days=365))
	def age(self):
		return datetime.datetime.now() - self.created

def getUserByToken(token: str):
	token=LoginToken.objects.get(token=token)

	if not token.expired():
		return token.user
	else:
		token.delete()
		raise ExpiredLoginToken(token.user,token.age())
