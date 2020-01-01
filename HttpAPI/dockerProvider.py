def getLocal():
	import dockerProviders.local
	return dockerProviders.local

def getAzure():
	import dockerProviders.azure
	return dockerProviders.azure

def getDummy():
	import dockerProviders.dummy
	return dockerProviders.dummy

getDockerProvider={
	'local':getLocal,
	'azure':getAzure,
	None:getDummy,
}
