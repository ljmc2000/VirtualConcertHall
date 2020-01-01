from databaseClasses import *
from os import environ
from bson import json_util
from flask import Flask,request,Response
app = Flask(__name__)

def jsonify(json: dict):
	return Response(json_util.dumps(json),mimetype='application/json')

@app.route("/register",methods=['POST'])
def register():
	try:
		r=request.get_json()
		u=User(username=r['username'])
		u.setpwd(r['password'])
		u.save()
		return jsonify({'status':'success'})
	except Exception as e:
		app.logger.error(e)
		return jsonify({'status':'failure'})

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
			return jsonify({'status':'failure','reason':'invalidPassword'})
	except Exception as e:
		app.logger.error(e)
		return jsonify({'status':'failure'})

@app.route("/createRoom",methods=['POST'])
def createRoom():
	try:
		r=request.get_json()
		owner=LoginToken.objects.get(token=r['token']).user
		room=Room(roomname=r['roomname'],owner=owner,description=r.get('description'))

		if r.get('private'):
			room.private=True
		if r.get('password'):
			room.setpwd(r['password'])

		room.save()
		return jsonify({'status':'success','roomId':str(room.id)})

	except Exception as e:
		app.logger.error(e)
		return jsonify({'status':'failure'})

@app.route("/listRooms",methods=['POST','GET'])
def listRooms():
	try:
		r=request.get_json()

		if r == None:
			rooms=Room.objects(active=True)

		returnme=[]
		for room in rooms:
			returnme.append({"roomId":room.id,"roomname":room.roomname, "owner": room.owner.username,"description":room.description})

		return jsonify({'status':'success','results':returnme})

	except Exception as e:
		app.logger.error(e)
		return jsonify({'status':'failure'})

if __name__ == "__main__":
	connect(environ['MONGO_URL'])
	app.run(debug=True)
