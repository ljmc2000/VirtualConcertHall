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

@app.route("/test",methods=['POST'])
def test():
	try:
		r=request.get_json()
		getUserByToken(r['token'])
		return jsonify({"status":"success"})
	except DoesNotExist as e:
		return jsonify({"status":"failure","reason":"A non existing login token was transmitted. It may have expired"})
	except Exception as e:
		return jsonify(handleException(app,e,'/test'))

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

@app.route("/createRoom",methods=['POST'])
def createRoom():
	try:
		r=request.get_json()
		owner=getUserByToken(r['token'])
		room=Room(roomname=r['roomname'],owner=owner,description=r.get('description'))

		if r.get('private'):
			room.private=True
		if r.get('password'):
			room.setpwd(r['password'])

		roomContainer=dockerProvider.startRoomContainer()
		room.ipaddress=IpAddress(ip=roomContainer['ip'],port=roomContainer['port'])
		room.containerid=roomContainer['id']

		room.save()
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
			room.delete()

			return jsonify({'status':'success'})
		else:
			return jsonify({'status':'failure','reason':'You do not currently have a room open'})

	except Exception as e:
		return jsonify(handleException(app,e,'/closeRoom'))


if __name__ == "__main__":
	app.run(debug=True)
