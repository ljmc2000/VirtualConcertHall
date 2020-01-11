from dockerProvider import getDockerProvider
from exceptions import handleException
from databaseClasses import *
from os import environ
from bson import json_util
from bson.objectid import ObjectId
from flask import Flask,request,Response

dockerProvider=getDockerProvider[environ.get('DOCKER_PROVIDER')]()
app = Flask(__name__)

def jsonify(json: dict):
	return Response(json_util.dumps(json),mimetype='application/json')

#common endpoints
@app.route("/test",methods=['POST'])
def test():
	try:
		r=request.get_json()
		getUserByToken(r['token'])
		return jsonify({"status":"success","invalid":False})
	except (DoesNotExist,ExpiredLoginToken) as e:
		return jsonify({"status":"success","invalid":True})
	except Exception as e:
		return jsonify(handleException(app,e,'/test'))

@app.route("/logout",methods=['POST'])
def logout():
	try:
		r=request.get_json()
		token=LoginToken.objects.get(token=r['token'])
		token.delete()

		return jsonify({"status":"success"})

	except Exception as e:
		return jsonify(handleException(app,e,'/logout'))

@app.route("/getUserStatus",methods=['POST'])
def getUserStatus():
	try:
		r=request.get_json()
		user=getUserByToken(r['token'])
		user.update(set__lastPing=datetime.datetime.now())
		user.lastPing=datetime.datetime.now()

		if r.get('username'):
			user=User.objects.get(username=r['username'])

		try:
			player=Player.objects.get(user=user)
		except DoesNotExist as e:
			player=None

		if user.lastPing + datetime.timedelta(minutes=15) < datetime.datetime.now():
			return jsonify({"status":"success","userStatus":"OFFLINE"})
		elif player != None:
			return jsonify({"status":"success","userStatus":"INROOM"})
		else:
			return jsonify({"status":"success","userStatus":"ONLINE"})

	except (DoesNotExist,ExpiredLoginToken) as e:
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

@app.route("/getUsername",methods=['POST'])
def getUsername():
	try:
		r=request.get_json()
		user=getUserByToken(r['token'])

		return jsonify({'status':'success','username':user.username})

	except Exception as e:
		return jsonify(handleException(app,e,'/getUsername'))

@app.route("/createRoom",methods=['POST'])
def createRoom():
	try:
		r=request.get_json()
		owner=getUserByToken(r['token'])
		token=LoginToken()
		token.save()
		room=Room(roomname=r['roomname'],owner=owner,players=[owner],description=r.get('description'),token=token)

		if r.get('private'):
			room.private=True
		if r.get('password'):
			room.setpwd(r['password'])


		roomContainer=dockerProvider.startRoomContainer(token.id)
		room.ipaddress=IpAddress(ip=roomContainer['ip'],port=roomContainer['port'])
		room.containerid=roomContainer['id']
		room.save()

		p=Player(user=owner,room=room)
		p.save()

		return jsonify({'status':'success','roomId':str(room.id)})

	except Exception as e:
		return jsonify(handleException(app,e,'/createRoom'))

@app.route("/listRooms",methods=['POST','GET'])
def listRooms():
	try:
		r=request.get_json()

		if r == None:
			rooms=Room.objects()

		returnme=[]
		for room in rooms:
			returnme.append({"roomId":str(room.id),"roomname":room.roomname, "owner": room.owner.username,"description":room.description})

		return jsonify({'status':'success','results':returnme})

	except Exception as e:
		return jsonify(handleException(app,e,'/listRooms'))

@app.route("/getCurrentRoom",methods=['POST'])
def getCurrentRoom():
	try:
		r=request.get_json()
		user=getUserByToken(r['token'])
		player=Player.objects.get(user=user)

		return jsonify({
				'status':'success',
				'roomIp':player.room.ipaddress.ip,
				'roomPort':player.room.ipaddress.port,
				'secretId':str(player.secretId)
		})

	except Exception as e:
		return jsonify(handleException(app,e,'/getCurrentRoom'))

@app.route("/joinRoom",methods=['POST'])
def joinRoom():
	try:
		r=request.get_json()
		user=getUserByToken(r['token'])
		room=Room.objects.get(id=r['roomId'])
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
		player=Player.objects.get(user=getUserByToken(r['token']))
		if player.room.owner.id != player.user.id:
			player.delete()
			return jsonify({'status':'success'})
		else:
			return jsonify({'status':'failure','reason':'A user may not leave their own room'})

	except DoesNotExist as e:
		return jsonify({'status':'failure', 'reason':'User is not in a room'})

	except Exception as e:
		return jsonify(handleException(app,e,'/joinRoom'))

@app.route("/closeRoom",methods=['POST'])
def closeRoom():
	try:
		r=request.get_json()
		u=getUserByToken(r['token'])
		room=Room.objects.get(owner=u)

		if room != None:
			dockerProvider.deleteRoomContainer(room['containerid'])
			closedRoom=ClosedRoom()
			closedRoom.fromRoom(room)
			closedRoom.save()
			Player.objects(room=room).delete()

			room.delete()

			return jsonify({'status':'success'})
		else:
			return jsonify({'status':'failure','reason':'You do not currently have a room open'})

	except Exception as e:
		return jsonify(handleException(app,e,'/closeRoom'))

#server endpoints
@app.route("/getClientId",methods=['POST'])
def getClientId():
	try:
		r=request.get_json()
		checkIfServerToken(r['token'])
		room=Room.objects.get(token=r['token'])
		player=Player.objects.get(secretId=int(r['secretId']),room=room)

		return jsonify({"status":"success","clientId":str(player.clientId)})

	except Exception as e:
		return jsonify(handleException(app,e,'/getClientId'))

@app.route("/timeoutRoom",methods=['POST'])
def timeoutRoom():
	try:
		r=request.get_json()
		checkIfServerToken(r['token'])
		room=Room.objects.get(token=r['token'])

		closedRoom=ClosedRoom()
		closedRoom.fromRoom(room)
		closedRoom.save()

		Player.objects(room=room).delete()
		room.delete()
		LoginToken.objects.get(token=r['token']).delete()

		return jsonify({"status":"success"})

	except Exception as e:
		return jsonify(handleException(app,e,'/timeoutRoom'))

if __name__ == "__main__":
	app.run(debug=True)
