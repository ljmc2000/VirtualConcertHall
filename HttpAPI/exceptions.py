from mongoengine.queryset import NotUniqueError,DoesNotExist
from werkzeug.exceptions import BadRequest
from pymongo.errors import ServerSelectionTimeoutError

import re	#my patience wears thin for people who write exceptions which include a message and nothing else so you must dig through the message with regular exceptions
notUniqueErrorRegex=re.compile('Tried to save duplicate unique keys (\(([\w\d]*) duplicate key error collection: \w*\.(\w*) .*)')
doesNotExistRegex=re.compile('(\w*) matching query does not exist')

class ExpiredLoginToken(Exception):
	def __init__(self,user,age):
		super().__init__("Login token for user %s is expired" % user.username)
		self.userid=user.id
		self.age=age

class ShortPassword(Exception):
	def __init__(self, length: int):
		from databaseClasses import passwordSize
		super().__init__("Passwords must be a minimum of %d characters but was %d" % (passwordSize,length))
		self.length = length

class BadPassword(Exception):
	def __init__(self, problem):
		super().__init__("Passwords must contain one each of a lowercase, uppercase, digit and symbol. Password provided fails on criteria '%s'" % problem)
		self.problem=problem

def handleException(app, e: Exception, endpoint: str):
	try:
		app.logger.error(type(e).__name__+' '+str(e))
		raise e
	except ExpiredLoginToken as e:
		return {"status":"failure","reason":"Your login token has expired"}
	except (ShortPassword, BadPassword) as e:
		return {"status":"failure","reason":str(e)}
	except NotUniqueError as e:
		usefulInformation=notUniqueErrorRegex.search(str(e))
		return {"status":"failure","reason":"A duplicate %s exists" % usefulInformation.group(3)}
	except DoesNotExist as e:
		usefulInformation=doesNotExistRegex.search(str(e))
		return {"status":"failure","reason":"No such %s exists" % usefulInformation.group(1)}
	except BadRequest as e:
		return {"status":"failure","reason":"A malformed or otherwise invalid request was made to the server"}
	except KeyError as e:
		return {"status":"failure","reason":"required fields were missing from the json request"}
	except ServerSelectionTimeoutError as e:
		return {"status":"failure","reason":"Remote application failed to connect to database"}
	except Exception as e:
		from databaseClasses import UnexpectedError
		error=UnexpectedError(type=type(e).__name__, message=str(e), endpoint=endpoint)
		error.save()
		return {"status":"failure","reason":"An unknown error has occured"}
