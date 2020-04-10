# Dependencies
The log analyzer requires matplotlib - pip install --user matplotlib
cli.py requires the mock client be built

# Usage
python cli.py register - register every single client in the config file  
python cli.py login - get a login token for every client and store it in tokens.txt  
python cli.py open - open an instance of the mock client for every user  
python cli.py list - get a list of available rooms from the HttpAPI  
python cli.py join x - join room number x where x is a room id  
python cli.py play - play everyone's midi files through their mock client  

python log\_analyzer.py /path/to/logs lowestLog(int) highestLog(int) - show a box chart of latencies  
logs may be created by running a real client with stderr writing out to file  

# Config
config.txt is a list of users in the form username, password, instrumentType, instrumentArgs, midifile name.  
tokens.txt is a list of usernames and their login token. It can be generated with the cli

