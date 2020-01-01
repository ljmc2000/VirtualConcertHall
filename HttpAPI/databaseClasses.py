import bcrypt
from mongoengine import *
from mongoengine.fields import *

connect('virtualconcerthall')

class WithPassword:
	def setpwd(self,password: str):
		self.passhash=bcrypt.hashpw(password.encode(),bcrypt.gensalt()).decode()

	def checkpwd(self, password: str):
		return bcrypt.checkpw(password.encode(), self.passhash.encode())

class User(Document,WithPassword):
	username = StringField(unique=True,required=True)
	passhash = StringField(max_length=60,required=True)

class Room(Document,WithPassword):
	roomname = StringField(required=True)
	passhash = StringField(max_length=60)
	description = StringField()
	active = BooleanField(required=True)

class LoginToken(Document):
	user=ReferenceField(User, required=True)

