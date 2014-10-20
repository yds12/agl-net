#ifndef _AGL_NET_H
#define _AGL_NET_H

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"

#include "agl/format.h"

#define AGLNET_MAX_PLAYERS 12
#define AGLNET_PACKET_SIZE 100
#define AGLNET_DEFAULT_PORT 1234

#define AGLNET_MSG_SERVER_INDEX 0
#define AGLNET_MSG_CLIENT_INDEX 1
#define AGLNET_MSG_PINFO_INDEX 2

#define AGLNET_MSG_SERVER "AGLNET_SERVER"
#define AGLNET_MSG_CLIENT "AGLNET_CLIENT"
#define AGLNET_MSG_PINFO "AGLNET_PINFO"

/// @mainpage AGL - Aleva Game Library
/// @section Libraries
/// AGL Libraries
/// - <a href="../../index.html">agl: main AGL Library.</a>
/// - <a href="../../agl-graphics/html/index.html">agl-graphics: AGL Graphics Library.</a>
/// - <a href="../../agl-mapping/html/index.html">agl-mapping: AGL Mapping Library.</a>
/// - <a href="../../agl-motion/html/index.html">agl-motion: AGL Motion Library.</a>
/// - <a href="../../agl-text/html/index.html">agl-text: AGL Text Library.</a>
/// - <a href="../../agl-forms/html/index.html">agl-forms: AGL Forms Library.</a>
/// - agl-net: AGL Net Library.

namespace AGL
{	
	/// Represents a player in the network
	struct NetPlayer
	{
		/// The player Id
		unsigned char id;
		
		/// The player Ip
		Uint32 ip;
		
		/// The player port
		Uint16 port;
		
		/// Whether the player is connected
		bool connected;
		
		/// Whether the player is a game server
		bool server;
		
		/// Whether the player slot is active (being used) 
		bool active;
		
		/// Time of the last message received by this player (in milliseconds after AGL Net initialization)
		int lastReceived;
	};

	/// Provides easy network messaging
	class Net
	{
		private:
			static UDPsocket udpSocket;
			static unsigned int packetSize;
			static unsigned short defaultPort;
			static bool* handleMessage;
		
		public:
			/// Maximum number of net players allowed
			static unsigned char maxPlayers;
			
			/// Players in the network
			static NetPlayer* players;
					
			/// Initialize the network
			///
			/// @return whether the network was successfully initialized
			static bool initializeNetwork();
					
			/// Initialize the network
			///
			/// @param autoAssign whether the port should be auto-assigned
			/// @return whether the network was successfully initialized
			static bool initializeNetwork(bool autoAssign);
			
			/// Initialize the network
			///
			/// @param port the default port
			/// @return whether the network was successfully initialized
			static bool initializeNetwork(unsigned short port);
			
			/// Initialize the network
			///
			/// @param port the default port
			/// @param _packetSize default size of the network packets
			/// @return whether the network was successfully initialized
			static bool initializeNetwork(unsigned short port, unsigned int _packetSize);
			
			/// Initialize the network, creating a UDP socket in a specified port
			///
			/// @param port the default port
			/// @param _packetSize default size of the network packets
			/// @param _maxPlayers maximum number of players in the network
			/// @return whether the network was successfully initialized
			static bool initializeNetwork(unsigned short port, unsigned int _packetSize, unsigned char _maxPlayers);
			
			/// Initialize the network, creating a UDP socket in a specified port
			///
			/// @param autoAssign whether the port should be auto-assigned
			/// @param port the default port
			/// @param _packetSize default size of the network packets
			/// @param _maxPlayers maximum number of players in the network
			/// @return whether the network was successfully initialized
			static bool initializeNetwork(bool autoAssign, unsigned short port, unsigned int _packetSize, unsigned char _maxPlayers);
			
			/// Closes the network, executing the necessary network procedures
			static void closeNetwork();
			
			/// Reset all the players' network data (ip, port, id, connected...)
			static void resetPlayers();
			
			/// Reads a message from the network (assuming the default message length)
			///
			/// @return the message in char* format
			static char* readMessage();
			
			/// Reads a message from the network (assuming the default message length)
			///
			/// @param buffer char* buffer to store the message content
			/// @return whether there was a message 
			static bool readMessage(char* buffer);
			
			/// Reads a message from the network
			///
			/// @param buffer char* buffer to store the message content
			/// @param length the message length
			/// @return whether there was a message 
			static bool readMessage(char* buffer, unsigned int length);
			
			/// Reads a message from the network
			///
			/// @param buffer char* buffer to store the message content
			/// @param sender struct with the sender data
			/// @return whether there was a message 
			static bool readMessage(char* buffer, NetPlayer* sender);
			
			/// Reads a message from the network
			///
			/// @param buffer char* buffer to store the message content
			/// @param length the message length
			/// @param sender struct with the sender data
			/// @return whether there was a message 
			static bool readMessage(char* buffer, unsigned int length, NetPlayer* sender);

			/// Sends a message to the network (with default length)
			///
			/// @param message message to send
			/// @param playerIndex index of the player in the network players' array
			static bool sendMessage(char* message, unsigned char playerIndex);

			/// Sends a message to the network
			///
			/// @param message message to send
			/// @param length the message length
			/// @param playerIndex index of the player in the network players' array
			/// @return whether the message was successfully sent
			static bool sendMessage(char* message, unsigned int length, unsigned char playerIndex);

			/// Sends a message to the network
			///
			/// @param message message to send
			/// @param player player to which the message will be sent
			/// @return whether the message was successfully sent
			static bool sendMessage(char* message, NetPlayer player);

			/// Sends a message to the network
			///
			/// @param message message to send
			/// @param length the message length
			/// @param player player to which the message will be sent
			/// @return whether the message was successfully sent
			static bool sendMessage(char* message, unsigned int length, NetPlayer player);

			/// Sends a message to all the active players
			///
			/// @param message message to send
			/// @return whether the message was successfully sent
			static bool sendToAllActive(char* message);

			/// Sends a message to all active servers
			///
			/// @param message message to send
			/// @return whether the message was successfully sent
			static bool sendToServers(char* message);

			/// Sends a message to all active non-server players
			///
			/// @param message message to send
			/// @return whether the message was successfully sent
			static bool sendToPlayers(char* message);

			/// Broadcasts a message in the network (with default length)
			///
			/// @param message the message
			/// @return whether the message was successfully sent
			static bool broadcastMessage(char* message);

			/// Broadcasts a message in the network
			///
			/// @param message the message
			/// @param length message length
			/// @return whether the message was successfully sent
			static bool broadcastMessage(char* message, unsigned int length);

			/// Add a new NetPlayer in an inactive slot (assuming it is active and connected). If the slots are full, return -1.
			///
			/// @param player player info to add
			/// @return the index where the player was added
			static int addPlayer(NetPlayer player);
				
			/// Add a new NetPlayer in an inactive slot (assuming it is active and connected). If the player already exists, 
			/// updates it, don't assuming it is active and connected. If the slots are full, return -1.
			///
			/// @param player player info to add or update
			/// @return the index where the player was added
			static int addOrUpdatePlayer(NetPlayer player);

			/// Return whether the network players' slots are full or not
			///
			/// @return whether the network players' slots are full or not
			static bool slotsFull();
			
			/// Sends all net players' information for all active players
			static void sendPlayersInfoToAll();
			
			/// Sends information of a specified net player for all active players
			///
			/// @param playerIndex index of the player whose information will be sent to all the others
			static void sendPlayerInfoToAll(int playerIndex);
			
			/// Handle the messages used by AGL Net to control players information among other things (if it is not a 
			/// AGL Net internal message, ignore it)
			/// 
			/// @param message message to handle
			/// @param sender NetPlayer that sent the message
			static void handleInternalMessage(char* message, NetPlayer sender);
			
			/// Enables and disables the internal message handling for each type of internal message
			///
			/// @param server whether the server internal message must be handled
			/// @param client whether the server internal message must be handled
			/// @param playerInfo whether the server internal message must be handled
			static void enableInternalMessageHandling(bool server, bool client, bool playerInfo);
			
			/// Gets a net player by his IP and port
			///
			/// @param ip IP to seek
			/// @param port port to seek
			/// @param player struct to fill
			/// @return whether the player was found or not
			static bool getPlayer(unsigned int ip, unsigned short port, NetPlayer &player);
	};
}

#endif

