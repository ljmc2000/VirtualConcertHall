from sys import argv
from os import makedirs, environ
import requests, subprocess

instruments={"PIANO":"0","GUITAR":"1","DRUM":"2",
				"0":"PIANO","1":"GUITAR","2":"DRUM"}
HTTPAPIURL="http://localhost:5000" if not environ.get("HTTPAPIURL") else environ.get("HTTPAPIURL")

players=[]
with open("config.txt","r") as file:
	for line in file:
		if(line[0]!='#'):
			split_line=line.strip().split(' ')
			players.append({"username":split_line[0],"password":split_line[1],"instrument":instruments[split_line[2]],"instrumentArgs":split_line[3],"midifile":split_line[4]})

tokens={}
try:
	with open("tokens.txt","r") as file:
		for line in file:
			split_line=line.strip().split(':')
			tokens[split_line[0]]=split_line[1]
except FileNotFoundError:
	pass

ports={}
getPorts=subprocess.run(["pmidi", "-l"], stdout=subprocess.PIPE)
for line in getPorts.stdout.decode().strip().split('\n'):
	line=line.strip().split()
	if(line[1]=="VirtualConcertHallMockClient"):
		ports[line[2]]=line[0]

def register(args):
	for player in players:
		r=requests.post(url=HTTPAPIURL+'/register',
			json={"username":player["username"],"password":player["password"]})
		if r.json()["status"] == "success":
			print("successfully registered", player["username"])
		else:
			print("error registering", player["username"])
			
def login(args):
	for player in players:
		r=requests.post(url=HTTPAPIURL+'/login',
			json={"username":player["username"],"password":player["password"]})
		if r.json()["status"] != "success":
			print("Warning: failed to login", player["username"])
		else:
			token=r.json()["token"]
			
			tokenfile=open("tokens.txt","a+")
			tokenfile.write(player["username"]+':'+token+'\n')
			tokenfile.close()

def join(args):
	for player in players:
		r=requests.post(url=HTTPAPIURL+'/joinRoom',
			json={"roomId":args[0]},
			headers={"loginToken":tokens[player["username"]]})

		print(r.json())

def listRooms(args):
	page=args[0] if len(args)>0 else 0
	perPage=args[1] if len(args)>1 else 10
	r=requests.post(url=HTTPAPIURL+'/listRooms',
		json={"page":page,"perPage":perPage})
	print(r.text)

def openApp(args):
	for player in players:
		env={"LD_LIBRARY_PATH":"../RoomClient/rtmidiLinux","HTTPAPIURL":HTTPAPIURL}
		for var in environ.keys():
			if not(env.get(var)):
				env[var]=environ[var]

		r=requests.get(url=HTTPAPIURL+'/getCurrentRoom',
			headers={"loginToken":tokens[player["username"]]})

		j=r.json()
		if j["status"]=="success":
			print("spawning process for",player["username"])
			subprocess.Popen(["../MockClient/MockClient",
									j["roomIp"],
									str(j["roomPort"]),
									str(j["roomId"]),
									j["secretId"],
									player["username"],
									player["instrument"],
									player["instrumentArgs"],
									],env=env)
		else:
			print(player["username"],"did not spawn process:",j["reason"])

def play(args):
	for player in players:
		if(ports.get(player["username"])):
			subprocess.Popen(["pmidi",'-d0',"-p",ports[player["username"]],"midi/"+player["midifile"]])

functions={
	"register":register,
	"play":play,
	"login":login,
	"join":join,
	"list":listRooms,
	"open":openApp,
}

if __name__ == "__main__":
	if(len(argv)>1):
		functions[argv[1]](argv[2:])
	else:
		print(functions.keys())
