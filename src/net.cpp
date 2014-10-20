#include "agl/net/net.h"

unsigned char AGL::Net::maxPlayers;
UDPsocket AGL::Net::udpSocket;
unsigned int AGL::Net::packetSize;
AGL::NetPlayer* AGL::Net::players;
unsigned short AGL::Net::defaultPort;
bool* AGL::Net::handleMessage;

bool AGL::Net::initializeNetwork()
{	
	return initializeNetwork(AGLNET_DEFAULT_PORT, AGLNET_PACKET_SIZE, AGLNET_MAX_PLAYERS);
}

bool AGL::Net::initializeNetwork(bool autoAssign)
{	
	return initializeNetwork(autoAssign, AGLNET_DEFAULT_PORT, AGLNET_PACKET_SIZE, AGLNET_MAX_PLAYERS);
}

bool AGL::Net::initializeNetwork(unsigned short port)
{	
	return initializeNetwork(port, AGLNET_PACKET_SIZE, AGLNET_MAX_PLAYERS);
}

bool AGL::Net::initializeNetwork(unsigned short port, unsigned int _packetSize)
{	
	return initializeNetwork(port, _packetSize, AGLNET_MAX_PLAYERS);
}

bool AGL::Net::initializeNetwork(unsigned short port, unsigned int _packetSize, unsigned char _maxPlayers)
{	
	return initializeNetwork(false, AGLNET_DEFAULT_PORT, _packetSize, _maxPlayers);
}	

bool AGL::Net::initializeNetwork(bool autoAssign, unsigned short port, unsigned int _packetSize, unsigned char _maxPlayers)
{	
	handleMessage = new bool[3];
	
	handleMessage[AGLNET_MSG_SERVER_INDEX] = true;
	handleMessage[AGLNET_MSG_CLIENT_INDEX] = true;
	handleMessage[AGLNET_MSG_PINFO_INDEX] = true;
	
	defaultPort = port;
	udpSocket = NULL;
	maxPlayers = _maxPlayers;
	resetPlayers();
	packetSize = _packetSize;
	bool ok = SDLNet_Init() >= 0;
	
	if(ok)
	{
		if(autoAssign) udpSocket = SDLNet_UDP_Open(0);
		else udpSocket = SDLNet_UDP_Open(port);
		
		if(!udpSocket) return false;
	}
	
	return ok;
}	

void AGL::Net::closeNetwork()
{
	SDLNet_UDP_Close(udpSocket);
	udpSocket = NULL;
}

void AGL::Net::resetPlayers()
{
	players = new NetPlayer[maxPlayers];
	for(int i = 0; i < maxPlayers; i++)
	{
		players[i].ip = 0;
		players[i].port = 0;
		players[i].id = 0;
		players[i].connected = false;
		players[i].server = false;
		players[i].active = false;
		players[i].lastReceived = 0;
	}
}

char* AGL::Net::readMessage()
{
	char buffer[packetSize];
	readMessage(buffer, packetSize, NULL);
	return buffer;
}	

bool AGL::Net::readMessage(char* buffer)
{
	return readMessage(buffer, packetSize, NULL);
}	

bool AGL::Net::readMessage(char* buffer, unsigned int length)
{
	return readMessage(buffer, length, NULL);
}	

bool AGL::Net::readMessage(char* buffer, NetPlayer* sender)
{
	return readMessage(buffer, packetSize, sender);
}	

bool AGL::Net::readMessage(char* buffer, unsigned int length, NetPlayer* sender)
{
	int received = 0;
	UDPpacket* in = NULL;

	if(udpSocket == NULL) return false;

	in = SDLNet_AllocPacket(length);
	received = SDLNet_UDP_Recv(udpSocket, in);

	if(received > 0)
	{
		strcpy(buffer, (char*)in->data);
		
		if(sender != NULL)
		{
			sender->ip = in->address.host;
			sender->port = in->address.port;
			sender->id = 0;
			sender->connected = true;
			sender->server = false;
		}
	}

	SDLNet_FreePacket(in);
	return received > 0;
}

bool AGL::Net::sendMessage(char* message, unsigned char playerIndex)
{
	return sendMessage(message, packetSize, playerIndex);
}

bool AGL::Net::sendMessage(char* message, NetPlayer player)
{
	return sendMessage(message, packetSize, player);
}

bool AGL::Net::sendMessage(char* message, unsigned int length, unsigned char playerIndex)
{
	return sendMessage(message, length, players[playerIndex]);
}

bool AGL::Net::sendMessage(char* message, unsigned int length, NetPlayer player)
{
	UDPpacket* out;
	int sent = 0;

	out = SDLNet_AllocPacket(length); 
	out->len = strlen(message) + 1;

	for(int i = 0; i < out->len; i++)
		out->data[i] = (Uint8) message[i];
		
	out->address.host = player.ip;
	out->address.port = player.port;

	sent = SDLNet_UDP_Send(udpSocket, -1, out);
	SDLNet_FreePacket(out);
	
	return sent > 0;
}

bool AGL::Net::sendToAllActive(char* message)
{
	for(int i = 0; i < maxPlayers; i++)
		if(players[i].active)
			sendMessage(message, i);
}

bool AGL::Net::sendToServers(char* message)
{
	for(int i = 0; i < maxPlayers; i++)
		if(players[i].active && players[i].server)
			sendMessage(message, i);
}

bool AGL::Net::sendToPlayers(char* message)
{
	for(int i = 0; i < maxPlayers; i++)
		if(players[i].active && !players[i].server)
			sendMessage(message, i);
}

bool AGL::Net::broadcastMessage(char* message)
{ 
	return broadcastMessage(message, packetSize);
}

bool AGL::Net::broadcastMessage(char* message, unsigned int length)
{ 
	UDPpacket* out;
	IPaddress broadcastIp;

	out = SDLNet_AllocPacket(length);
	SDLNet_ResolveHost(&broadcastIp, "255.255.255.255", defaultPort);
	out->address.host = broadcastIp.host;
	out->address.port = broadcastIp.port;
	out->len = strlen(message) + 1;
	
	for(int i = 0; i < out->len; i++)
		out->data[i] = (Uint8) message[i];
	
	int sent = SDLNet_UDP_Send(udpSocket, -1, out);

	return sent > 0;
}

bool AGL::Net::slotsFull()
{
	for(int p = 0; p < maxPlayers; p++)
		if(!AGL::Net::players[p].active)
			return true;
			
	return false;
}

int AGL::Net::addPlayer(NetPlayer player)
{
	bool notYetAdded = true;
	int empty = -1;
	
	for(int p = 0; p < maxPlayers; p++)
	{
		if(empty < 0 && !AGL::Net::players[p].active) empty = p;
		if(AGL::Net::players[p].ip == player.ip && AGL::Net::players[p].port == player.port)
		{ 
			notYetAdded = false;
			break;
		}
	}
	
	if(notYetAdded && empty >= 0)
	{
		AGL::Net::players[empty].ip = player.ip; 
		AGL::Net::players[empty].port = player.port; 
		AGL::Net::players[empty].id = player.id;
		AGL::Net::players[empty].connected = true;
		AGL::Net::players[empty].server = player.server;
		AGL::Net::players[empty].active = true;
		return empty;
	}
	
	return -1;
}

int AGL::Net::addOrUpdatePlayer(NetPlayer player)
{
	bool notYetAdded = true;
	int slot = -1;
	
	for(int p = 0; p < maxPlayers; p++)
	{
		if(slot < 0 && !AGL::Net::players[p].active) slot = p;
		if(AGL::Net::players[p].ip == player.ip && AGL::Net::players[p].port == player.port)
		{
			notYetAdded = false;
			slot = p;
			break;
		}
	}
	
	if(!notYetAdded)
	{
		AGL::Net::players[slot].ip = player.ip; 
		AGL::Net::players[slot].port = player.port; 
		AGL::Net::players[slot].id = player.id;
		AGL::Net::players[slot].connected = player.connected;
		AGL::Net::players[slot].server = player.server;
		AGL::Net::players[slot].active = player.active;
		AGL::Net::players[slot].lastReceived = player.lastReceived;
	}
	else if(slot >= 0)
	{
		AGL::Net::players[slot].ip = player.ip; 
		AGL::Net::players[slot].port = player.port; 
		AGL::Net::players[slot].id = player.id;
		AGL::Net::players[slot].connected = true;
		AGL::Net::players[slot].server = player.server;
		AGL::Net::players[slot].active = true;
		AGL::Net::players[slot].lastReceived = 0;
	}
	
	return slot;
}

void AGL::Net::sendPlayersInfoToAll()
{		
	char playerInfo[50];

	for(int i = 0; i < maxPlayers; i++)
	{
		sprintf(playerInfo, "%s %d %d %d %d %d %d %d", AGLNET_MSG_PINFO, 
			AGL::Net::players[i].ip, AGL::Net::players[i].port, AGL::Net::players[i].id, AGL::Net::players[i].connected,
			AGL::Net::players[i].server, AGL::Net::players[i].active, AGL::Net::players[i].lastReceived);
		
		AGL::Net::sendToAllActive(playerInfo);
	}
}

void AGL::Net::sendPlayerInfoToAll(int playerIndex)
{
	char playerInfo[50];

	sprintf(playerInfo, "%s %d %d %d %d %d %d %d", AGLNET_MSG_PINFO, 
		AGL::Net::players[playerIndex].ip, AGL::Net::players[playerIndex].port, AGL::Net::players[playerIndex].id,
		AGL::Net::players[playerIndex].connected,	AGL::Net::players[playerIndex].server, 
		AGL::Net::players[playerIndex].active, AGL::Net::players[playerIndex].lastReceived);
	
	AGL::Net::sendToAllActive(playerInfo);
}

void AGL::Net::handleInternalMessage(char* message, NetPlayer sender)
{
	char type[20];
	sscanf(message, "%s", &type);

	if(handleMessage[AGLNET_MSG_CLIENT_INDEX] && Format::compareChars(type, (char*)AGLNET_MSG_CLIENT))
	{
		int index = AGL::Net::addPlayer(sender);
		AGL::Net::players[index].id = index;
		AGL::Net::sendMessage(AGLNET_MSG_SERVER, index);
	}
	else if(handleMessage[AGLNET_MSG_SERVER_INDEX] && Format::compareChars(type, (char*)AGLNET_MSG_SERVER))
	{
		sender.server = true;		
		AGL::Net::addPlayer(sender);
	}
	else if(handleMessage[AGLNET_MSG_PINFO_INDEX] && Format::compareChars(type, (char*)AGLNET_MSG_PINFO))
	{
		AGL::NetPlayer player;
		sscanf(message, "%s %d %d %d %d %d %d %d", &type, 
					&player.ip, &player.port, &player.id, &player.connected, &player.server, &player.active, &player.lastReceived);
		AGL::Net::addOrUpdatePlayer(player);
	}
}

void AGL::Net::enableInternalMessageHandling(bool server, bool client, bool playerInfo)
{
	handleMessage[AGLNET_MSG_SERVER_INDEX] = server;
	handleMessage[AGLNET_MSG_CLIENT_INDEX] = client;
	handleMessage[AGLNET_MSG_PINFO_INDEX] = playerInfo;
}

bool AGL::Net::getPlayer(unsigned int ip, unsigned short port, NetPlayer &player)
{
	for(int p = 0; p < AGL::Net::maxPlayers; p++)
	{
		if(AGL::Net::players[p].ip == ip && AGL::Net::players[p].port == port) 
		{
			player = AGL::Net::players[p];
			return true;
		}
	}
	
	return false;
}

