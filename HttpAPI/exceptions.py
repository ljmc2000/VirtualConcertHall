from mongoengine.queryset import NotUniqueError


import re	#my patience wears thin for people who write exceptions which include a message and nothing else so you must dig through the message with regular exceptions
notUniqueErrorRegex=re.compile('Tried to save duplicate unique keys (\(([\w\d]*) duplicate key error collection: \w*\.(\w*) .*)')

class ExpiredLoginToken(Exception):
	def __init__(self,user,age):
		super().__init__("Login token for user %s is expired" % user.username)
		self.userid=user.id
		self.age=age

def handleException(e: Exception):
	try:
		raise e
	except ExpiredLoginToken:
		return {"status":"failure","reason":"Your login token has expired"}
	except NotUniqueError as e:
		usefulInformation=notUniqueErrorRegex.search(str(e))
		return {"status":"failure","reason":"A duplicate %s exists" % usefulInformation.group(3)}
	except Exception:
		return {"status":"failure","reason":"an unknown error has occured"}
