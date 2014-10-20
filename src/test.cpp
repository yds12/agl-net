#include <stdlib.h>
#include "agl/game.h"
#include "agl/drawer.h"

// ------------ Includes here ---------------
#include <iostream>
#include "agl/vector2.h"
#include "agl/intvector2.h"
#include "agl/gameobject.h"
#include "agl/support.h"
#include "agl/color.h"
#include "agl/net/net.h"
// ------------------------------------------

// ------------ Defines here ----------------
#define GS_ROOM 0
#define GS_GAME 1
// ------------------------------------------

AGL::Game* game = NULL;
AGL::Drawer* drawer = NULL;

// ------------ Declare variables here ------

int gameState;

int connectedPlayers;
AGL::IntVector2* positions; 
bool server;

char* MSG_NUMPLAYERS = "PLAYERS";
char* MSG_POS = "POS";
char* MSG_START = "START";
char* MSG_REQMOVE = "MOVE";

const string contentPath = "/home/USER/test/";

AGL::Image** imagem = NULL;
AGL::IntVector2* movement;
AGL::Keyboard* kb = NULL;
AGL::Mouse* ms = NULL;
AGL::Font* font = NULL;
// ------------------------------------------


void initialize()
{
	// ------------ Initialize variables here ---
	game = new AGL::Game;
	drawer = new AGL::Drawer;

	game->showMouse(true);
	kb = new AGL::Keyboard(false, false, true, true, false, false);
	ms = new AGL::Mouse;

	movement = new AGL::IntVector2(0, 0);
	positions = NULL;

	// Sala do jogo (aceitando novos players)
	gameState = GS_ROOM;
	
	// Inicia rede
	printf("initializing...\n");
	if(server)
	{
		if(AGL::Net::initializeNetwork(false)) printf("initialized\n");
		else printf("initialization failed!\n");
		
		AGL::Net::players[0].connected = true;
		AGL::Net::players[0].server = true;
		AGL::Net::players[0].active = true;
		
		AGL::Net::enableInternalMessageHandling(false, true, false);		
		game->setWindowTitle("AGL Net Game - Server");
	}
	else
	{
		if(AGL::Net::initializeNetwork(true)) printf("initialized\n");
		else printf("initialization failed!\n");
		
		AGL::Net::enableInternalMessageHandling(true, false, true);		
		game->setWindowTitle("AGL Net Game - Client");
	}
	
	imagem = new AGL::Image*[AGL::Net::maxPlayers];
	// ------------------------------------------
}

void loadContent()
{	
	// ------------ Load content here -----------
	imagem[0] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "thief.png")));
	imagem[1] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "bat.png")));
	imagem[2] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "b1.png")));
	imagem[3] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "f1.png")));
	imagem[4] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "f3.png")));
	imagem[5] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "l2.png")));
	imagem[6] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "grass.png")));
	imagem[7] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "mud.png")));
	imagem[8] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "water.png")));
	imagem[9] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "sand.png")));
	imagem[10] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "test.png")));
	imagem[11] = new AGL::Image(AGL::Format::stringToChar(&(contentPath + "ground.png")));
	font = new AGL::Font(AGL::Format::stringToChar(&(contentPath + "font.ttf")), 20);
	// ------------------------------------------
}

void handleMessage(char* message, AGL::NetPlayer sender)
{
	AGL::Net::handleInternalMessage(message, sender);
	
	char type[20];
	sscanf(message, "%s", &type);

	if(AGL::Format::compareChars(type, MSG_POS))
	{
		int player = 0, x = 0, y = 0;
		sscanf(message, "%s %d %d %d", &type, &player, &x, &y);
		printf("received: package %s (data %d %d %d) from %d %d\n", type, player, x, y, sender.ip, sender.port);

		positions[player].x = x;
		positions[player].y = y;
	}
	else if(AGL::Format::compareChars(type, MSG_NUMPLAYERS))
	{
		int pc;
		sscanf(message, "%s %d", &type, &pc);
		printf("received: package %s (data %d) from %d %d\n", type, pc, sender.ip, sender.port);

		connectedPlayers = pc;
		positions = new AGL::IntVector2[connectedPlayers];
	}
	else if(AGL::Format::compareChars(type, MSG_REQMOVE))
	{
		int x = 0, y = 0;
		sscanf(message, "%s %d %d", &type, &x, &y);
		printf("received: package %s (data %d %d) from %d %d\n", type, x, y, sender.ip, sender.port);
		
		AGL::Net::getPlayer(sender.ip, sender.port, sender);		
		printf("sender id: %d\n", sender.id);
		positions[sender.id].x += x;
		positions[sender.id].y += y;
	}
	else if(AGL::Format::compareChars(type, MSG_START))
	{
		AGL::Net::enableInternalMessageHandling(false, false, true);
		printf("received: package %s from %d %d\n", type, sender.ip, sender.port);
		printf("starting game\n");
		gameState = GS_GAME;
	}
}

void update()
{
	game->update(kb, ms, true);

	// ------------ Update the game here --------
	movement->x = 0;
	movement->y = 0;
	
	if(kb->isKeyPressed(kb->getIndex(AGL::KEY_UP))) movement->y = -30;
	if(kb->isKeyPressed(kb->getIndex(AGL::KEY_DOWN))) movement->y = 30;
	if(kb->isKeyPressed(kb->getIndex(AGL::KEY_LEFT))) movement->x = -30;
	if(kb->isKeyPressed(kb->getIndex(AGL::KEY_RIGHT))) movement->x = 30;
	if(kb->isKeyHeld(kb->getIndex(AGL::KEY_UP))) movement->y = -1;
	if(kb->isKeyHeld(kb->getIndex(AGL::KEY_DOWN))) movement->y = 1;
	if(kb->isKeyHeld(kb->getIndex(AGL::KEY_LEFT))) movement->x = -1;
	if(kb->isKeyHeld(kb->getIndex(AGL::KEY_RIGHT))) movement->x = 1;
	if(kb->isKeyDown(kb->getIndex(AGL::KEY_ESC))) game->quit = true;
	
	// Leave game room and start game
	if(server && kb->isKeyPressed(kb->getIndex(AGL::KEY_ENTER))) 
	{
		//printf("starting game\n");
		gameState = GS_GAME;
		
		connectedPlayers = 0;
		
		for(int p = 0; p < AGL::Net::maxPlayers; p++)
			if(AGL::Net::players[p].connected)
			{
				connectedPlayers++;
				if(!AGL::Net::players[p].server)
				{
					AGL::Net::sendMessage(MSG_START, p);
					//printf("sending %s to %d %d\n", MSG_START, AGL::Net::players[p].ip, AGL::Net::players[p].port);
				}
			}
		
		positions = new AGL::IntVector2[connectedPlayers];
		
		char buffer[99];
		sprintf(buffer, "%s %d", MSG_NUMPLAYERS, connectedPlayers);	
				
		for(int p = 0; p < AGL::Net::maxPlayers; p++)
			if(AGL::Net::players[p].connected && !AGL::Net::players[p].server)
			{
				AGL::Net::sendMessage(buffer, p);
				//printf("sending %s to %d %d\n", buffer, AGL::Net::players[p].ip, AGL::Net::players[p].port);
			}
						
		AGL::Net::enableInternalMessageHandling(false, false, false);
	}
	
	if(gameState == GS_ROOM)
	{
		char buf[100];
		AGL::NetPlayer player;
		
		if(server)
		{			
			if(AGL::Net::readMessage(buf, &player)) handleMessage(buf, player);
		} 
		else 
		{
			AGL::Net::broadcastMessage(AGLNET_MSG_CLIENT);
			SDL_Delay(50);
			if(AGL::Net::readMessage(buf, &player)) handleMessage(buf, player);
		}
	}
	else if(gameState == GS_GAME)
	{		
		if(server)
		{		
			//printf("server position: %d %d movement: %d %d\n", positions[0].x, positions[0].y, movement->x, movement->y);
			positions[0].x += movement->x;
			positions[0].y += movement->y;
			
			char buf[100];
			AGL::NetPlayer player;
			if(AGL::Net::readMessage(buf, &player)) handleMessage(buf, player);
		
			if(game->gameCounter == 0)	AGL::Net::sendPlayersInfoToAll();
				
			if(game->gameCounter % 10 == 0)
			{ 
				char posstr[50];
				for(int i = 0; i < AGL::Net::maxPlayers; i++)
				{
					if(AGL::Net::players[i].active)
					{
						int id = AGL::Net::players[i].id;
						sprintf(posstr, "%s %d %d %d", MSG_POS, id, positions[id].x, positions[id].y);
						AGL::Net::sendToAllActive(posstr);
						//printf("sending %s to all\n", posstr);
					}
				}
			}
		}
		else
		{
			char buf[100];
			AGL::NetPlayer player;
			
			for(int i = 0; i < 24; i++)
				if(AGL::Net::readMessage(buf, &player)) handleMessage(buf, player);
			
			if(movement->x != 0 || movement->y != 0)
			{
				sprintf(buf, "%s %d %d", MSG_REQMOVE, movement->x, movement->y);						
				AGL::Net::sendToServers(buf);
				//printf("sending %s to servers\n", buf);
			}
		}
	}
	// ------------------------------------------
}

void draw()
{
	drawer->clearScreen(AGL_COLOR_CORNFLOWERBLUE);

	// ------------ Draw objects here -----------	
	if(gameState == GS_GAME)
	{
		for(int i = 0; i < connectedPlayers; i++)
			drawer->draw(imagem[i], positions[i]);
	}
	else if(gameState == GS_ROOM)
	{
		char* text;
		if(server) text = "Waiting clients to connect... (PRESS ENTER TO START GAME)";
		else text = "Waiting servers...";
		
		drawer->drawText(font, text, *movement, AGL_COLOR_BLACK);
	}
	// ------------------------------------------

	drawer->updateScreen();
}

void unloadContent()
{
	// ------------ Unload objects here ---------
	
	// ------------------------------------------

	game->end();
}

/// Main method, initializes the program
int main(int argc, char *argv[])
{
	if(argc > 1 && AGL::Format::compareCharString(argv[1], "-s")) { server = true; }
	else {server = false; }
	
	initialize();
	loadContent();	
	
	game->runFixedFps(update, draw, 60);
	
	unloadContent();

	return 0;
}

