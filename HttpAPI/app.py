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
	except:
		return jsonify({'status':'failure'})

@app.route("/login",methods=['POST'])
def login():
	try:
		r=request.get_json()
		u=User.objects.get(username=r['username'])
		t=LoginToken(user=u)
		t.save()
		return jsonify({'status':'success','token':t.id})
	except Exception as e:
		app.debug(e)
		return jsonify({'status':'failure'})

if __name__ == "__main__":
	connect(environ['MONGO_URL'])
	app.run(debug=True)
