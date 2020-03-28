from sys import argv
from os import makedirs, environ
import requests, subprocess

instruments={"PIANO":"0","GUITAR":"1","DRUM":"2"}
HTTPAPIURL="http://localhost:5000"
HOMEDIR="/tmp/home/"

players=[]
with open("config.txt","r") as file:
	for line in file:
		if(line[0]!='#'):
			split_line=line.strip().split(' ')
			players.append({"username":split_line[0],"password":split_line[1],"instrument":instruments[split_line[2]],"midifile":split_line[3]})

ports={}
getPorts=subprocess.run(["pmidi", "-l"], stdout=subprocess.PIPE)
for line in getPorts.stdout.decode().strip().split('\n'):
	line=line.strip().split()
	if(line[1]=="VirtualConcertHall"):
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
			target=HOMEDIR+player["username"]+"/.config/TUblin/"
			try:
				out=open(target+"VirtualConcertHall.conf","w+")
			except:
				makedirs(target)
				out=open(target+"VirtualConcertHall.conf","w+")
			
			out.write('[General]\n')
			out.write('audioDriver=pulseaudio\n')
			out.write('instrumentArgsDRUM=0\n')
			out.write('instrumentArgsGUITAR=0\n')
			out.write('instrumentArgsPIANO=6024\n')
			out.write('instrumentType='+player["instrument"]+"\n")
			out.write('loginToken='+token+"\n")
			out.write('midiInPort=0\n')

def openApp(args):
	for player in players:
		env={"LD_LIBRARY_PATH":"../RoomClient/rtmidiLinux","HTTPAPIURL":HTTPAPIURL,"HOME":HOMEDIR+player["username"]}
		for var in environ.keys():
			if not(env.get(var)):
				env[var]=environ[var]
		subprocess.Popen(["../RoomClient/Client"],env=env)

def play(args):
	for player in players:
		if(ports.get(player["username"])):
			subprocess.Popen(["pmidi",'-d0',"-p",ports[player["username"]],"midi/"+player["midifile"]])

functions={
	"register":register,
	"play":play,
	"login":login,
	"open":openApp,
}

if __name__ == "__main__":
	if(len(argv)>1):
		functions[argv[1]](argv[2:])
	else:
		print(functions.keys())
