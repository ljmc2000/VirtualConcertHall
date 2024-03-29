from exceptions import handleException
from databaseClasses import *
from roomserver_util import *
from bson import json_util
from bson.objectid import ObjectId
from time import sleep
from flask import Flask,request,Response

app = Flask(__name__)

def jsonify(json: dict):
	return Response(json_util.dumps(json),mimetype='application/json')

#common endpoints
@app.route("/test",methods=['GET'])
def test():
	try:
		u=getUserByToken(request.headers['loginToken'])
		return jsonify({"status":"success","invalid":False})
	except (DoesNotExist,ExpiredLoginToken) as e:
		return jsonify({"status":"success","invalid":True})
	except Exception as e:
		return jsonify(handleException(app,e,'/test'))

@app.route("/logout",methods=['GET'])
def logout():
	try:
		token=LoginToken.objects.get(token=request.headers['loginToken'])
		token.delete()

		return jsonify({"status":"success"})

	except Exception as e:
		return jsonify(handleException(app,e,'/logout'))

@app.route("/getUserStatus",methods=['POST'])
def getUserStatus():
	try:
		r=request.get_json()
		user=getUserByToken(request.headers['loginToken'])
		user.update(set__lastPing=datetime.datetime.now())
		user.lastPing=datetime.datetime.now()

		if r.get('username'):
			user=User.objects.get(username=r['username'])

		try:
			player=Player.objects.get(user=user)
			if not testRoom(player.room):
				informServerOfUpdate(player.room.server)
				return jsonify({"status":"success","userStatus":"WAITING"})
		except DoesNotExist as e:
			player=None
		except socket.gaierror as e:
			app.logger.error(e)
			player=None

		if user.lastPing + datetime.timedelta(minutes=15) < datetime.datetime.now():
			return jsonify({"status":"success","userStatus":"OFFLINE"})
		elif player != None:
			return jsonify({"status":"success","userStatus":"INROOM"})
		else:
			return jsonify({"status":"success","userStatus":"ONLINE"})

	except (DoesNotExist,ExpiredLoginToken,KeyError) as e:
		return jsonify({"status":"success","userStatus":"NOLOGIN"})

	except Exception as e:
		json=handleException(app,e,'/getUserStatus')
		json["userStatus"]="APIDOWN"
		return jsonify(json)

#client endpoints
@app.route("/register",methods=['POST'])
def register():
	try:
		r=request.get_json()
		u=User(username=r['username'])
		u.setpwd(r['password'])
		u.save()
		return jsonify({'status':'success'})
	except ValidationError as e:
		return jsonify({'status':'failure','reason':'Username is too short'})
	except Exception as e:
		return jsonify(handleException(app,e,'/register'))

@app.route("/login",methods=['POST'])
def login():
	try:
		r=request.get_json()
		u=User.objects.get(username=r['username'])
		if u.checkpwd(r['password']):
			t=LoginToken(user=u)
			t.save()
			return jsonify({'status':'success','token':t.id})
		else:
			return jsonify({'status':'failure','reason':'Invalid password'})
	except Exception as e:
		return jsonify(handleException(app,e,'/login'))

@app.route("/getUsername",methods=['GET'])
def getUsername():
	try:
		user=getUserByToken(request.headers['loginToken'])

		return jsonify({'status':'success','username':user.username})

	except Exception as e:
		return jsonify(handleException(app,e,'/getUsername'))

@app.route("/getUsername",methods=['POST'])
def getUsernameByClientId():
	try:
		getUserByToken(request.headers['loginToken'])
		r=request.get_json()
		player=Player.objects.get(clientId=int(r['clientId']))

		return jsonify({'status':'success','username':player.user.username})

	except Exception as e:
		return jsonify(handleException(app,e,'/getUsername(byClientId)'))

@app.route("/createRoom",methods=['POST'])
def createRoom():
	try:
		r=request.get_json()
		owner=getUserByToken(request.headers['loginToken'])
		room=Room(roomname=r['roomname'],owner=owner,players=[owner],description=r.get('description'))

		if r.get('private'):
			room.private=True
		if r.get('password'):
			room.setpwd(r['password'])

		p=Player(user=owner,room=room)
		room.ownerId=p.secretId

		room.server.save()
		room.save()
		p.save()

		sleep(.25)	#give mongo a chance to write
		informServerOfUpdate(room.server)
		return jsonify({'status':'success','roomId':str(room.id)})

	except Exception as e:
		return jsonify(handleException(app,e,'/createRoom'))

@app.route("/listRooms",methods=['POST'])
def listRooms():
	try:
		r=request.get_json()
		perPage=r['perPage']
		page=r['page']
		rooms=Room.objects()

		returnme=[]
		for room in rooms.skip(page*perPage).limit(perPage):
			returnme.append({"roomId":str(room.id),"roomname":room.roomname, "owner": room.owner.username,"description":room.description,"password":bool(room.passhash)})

		return jsonify({'status':'success','results':returnme,'more':(perPage*(page+1)<rooms.count())})

	except Exception as e:
		return jsonify(handleException(app,e,'/listRooms'))

@app.route("/getCurrentRoom",methods=['GET'])
def getCurrentRoom():
	try:
		user=getUserByToken(request.headers['loginToken'])
		player=Player.objects.get(user=user)

		return jsonify({
			'status':'success',
			'roomIp':player.room.server.ip,
			'roomPort':player.room.server.port,
			'roomId':player.room.roomId,
			'secretId':str(player.secretId),
			'owner':player.user==player.room.owner,
		})

	except Exception as e:
		return jsonify(handleException(app,e,'/getCurrentRoom'))

@app.route("/joinRoom",methods=['POST'])
def joinRoom():
	try:
		r=request.get_json()
		user=getUserByToken(request.headers['loginToken'])

		room=Room.objects.get(roomId=r['roomId'])
		if room.passhash and not room.checkpwd(r['password']):
			return jsonify({'status':'failure', 'reason':'Incorrect password'})

		room.players.append(user)
		room.save()
		p=Player(user=user,room=room)
		p.save()

		return jsonify({'status':'success'})

	except NotUniqueError as e:
		return jsonify({'status':'failure', 'reason':'User is already in a room'})

	except Exception as e:
		return jsonify(handleException(app,e,'/joinRoom'))

@app.route("/leaveRoom",methods=['POST'])
def leaveRoom():
	try:
		r=request.get_json()
		player=Player.objects.get(user=getUserByToken(request.headers['loginToken']))
		if player.room.owner.id != player.user.id:
			player.delete()
			return jsonify({'status':'success'})
		else:
			return jsonify({'status':'failure','reason':'A user may not leave their own room'})

	except DoesNotExist as e:
		return jsonify({'status':'failure', 'reason':'User is not in a room'})

	except Exception as e:
		return jsonify(handleException(app,e,'/joinRoom'))

#server endpoints
@app.route("/getClientId",methods=['POST'])
def getClientId():
	try:
		r=request.get_json()
		checkIfServerToken(request.headers['loginToken'])
		player=Player.objects.get(secretId=int(r['secretId']),room=r['roomId'])

		return jsonify({"status":"success","clientId":str(player.clientId)})

	except Exception as e:
		return jsonify(handleException(app,e,'/getClientId'))

@app.route("/closeRoom",methods=['POST'])
def closeRoom():
	try:
		r=request.get_json()
		checkIfServerToken(request.headers['loginToken'])
		room=Room.objects.get(roomId=r['roomId'])
		server=room.server

		room.close(r['reason'])
		informServerOfUpdate(server)
		return jsonify({"status":"success"})

	except ValidationError as e:
		return jsonify({"status":"failure","reason":"Invalid reason for closure"})

	except Exception as e:
		return jsonify(handleException(app,e,'/closeRoom'))

@app.route("/refreshRooms",methods=['POST'])
def refreshRooms():
	try:
		r=request.get_json()
		checkIfServerToken(request.headers['loginToken'])
		server=RoomServer.objects.get(token=request.headers['loginToken'])

		returnme=[]
		known=r['known']

		for room in Room.objects(server=server):
			try:
				i=known.index(str(room.id))
				known.pop(i)
			except ValueError as e:
				returnme.append({"type":"ADD","roomId":room.roomId,"owner":room.ownerId})

		for roomId in known:
			returnme.append({"type":"REMOVE","roomId":roomId,"owner":0})

		return jsonify({"status":"success","updates":returnme})

	except Exception as e:
		return jsonify(handleException(app,e,'/refreshRooms'))

if __name__ == "__main__":
	app.run(debug=True)
