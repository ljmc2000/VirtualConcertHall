import re
from sys import argv
import matplotlib.pyplot as plt

packet_message=re.compile('(\\d{1,10}) "\\d{1,3}:\\d{1,3}:\\d{1,3}:" \\d{1,3} (\\d{1,3}) ms')
drop_packet_message=re.compile('Midi packet dropped from (\\d{1,10})')

data=[]
for i in range(int(argv[2]),int(argv[3])+1):
	d=[]
	with open(argv[1]+str(i)) as infile:
		for line in infile:
			packet = packet_message.match(line)
			if packet:
				d.append(int(packet.group(2)))
	data.append(d)

plt.boxplot(data,showfliers=False)
plt.show()
