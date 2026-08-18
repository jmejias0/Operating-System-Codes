/* 
 * Name: Jose Mejias
 * CWID: 820507572
 * Email: jmejias0@csu.fullerton.edu
 */

/**
 * 
 * Most of your work goes here.
 * 
 * Use this file to implement the functions declared in Chat.hpp
 * 
 * Do not place implementation into Chat.hpp; That's what this file is for.
 * 
 */

//
#include "./Chat.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <cerrno>
#include <pthread.h>
#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <cstring>


//	TODO: More includes? included 


///	Main namespace for this class
namespace CPSC351
{
	/*************************************************************
	 * Begin non-member functions, or regular "global" functions *
	 *************************************************************/
	
	/**
	 * 
	 * Non-member function (regular function) that will
	 * 	serve as an entry point for the sender thread.
	 * 
	 * Once inside this function, re-cast 'param' to a 'Chat*'
	 * 	and use it to call on a member function for your Chat
	 * 	instance.
	 * 
	 * This is a pattern sometimes used to wrap C++ class objects
	 * 	around C functions.
	 * 
	 * This function has been completed for you.
	 * 
	 * You must complete c_receiverThread on your own
	 * 
	 * (your welcome)
	 * 
	 */
	void* c_senderThread(void* param)
	{
		//	Grab the 'this' pointer, so we can get back inside our instance
		Chat* this_ = static_cast<Chat*>(param);
		this_->senderThread();
		
		return NULL;
	}
	
	/**
	 * Same idea as c_senderThread, but this should call
	 * 	the Chat::receiverThread() method.
	 */
	void* c_receiverThread(void* param)
	{
		Chat* this_ = static_cast<Chat*>(param);
		this_->receiverThread();

		return NULL;
	}
	
	/***********************************************************
	 * End non-member functions, or regular "global" functions *
	 ***********************************************************/

	/**
	 * Constructor
	 * 
	 * Remember the listening port, the outgoing hostname
	 * 	and the outgoing port.
	 */
	Chat::Chat(int port_listen, std::string hostname, int port_out)
	{
		this->port_listen_ = port_listen;
		this->hostname_ = hostname;
		this->port_out_ = port_out;
	}
	
	/**
	 * Destructor
	 * 
	 * Perform appropriate cleanup operations here, if any
	 * 
	 * Try closing the sockets here
	 */
	Chat::~Chat()
	{
		this->closeListeningSocket();
		this->closeOutboundSocket();
	}
	
	/**
	 * The run() function starts the chat session with these basic steps:
	 * 
	 * 1. Call the two functions that create sockets.
	 * 
	 * 2. Call this->spawnThreads(),
	 *      which will in turn spawn the sender and receiver threads,
	 *      and save the thread IDs to member variables.
	 * 
	 * 3. Call this->joinThreads(), which will join the threads.
	 * 
	 */
	void Chat::run()
	{
		this->initListeningSocket();
		this->initOutboundSocket();

		this->spawnThreads();
		this->joinThreads();

		std::cout << "Program exiting ..." << std::endl;
	}
	
	/**
	 * 
	 * Return true if the listening socket is "valid"; false otherwise
	 * 
	 * Probably want to rely on isSocketValid() to avoid repeating code
	 * 
	 */
	bool Chat::isListeningSocketValid() const
	{
		return this->isSocketValid(this->socket_listen_);
	}
	
	/**
	 * Initialize/create the listening socket that will receive data
	 *   from your chat partner.
	 * 
	 * You'll want to do the following things:
	 * 
	 * 1. Close the listening socket by calling closeListeningSocket(),
	 *      just in case its already open.
	 * 
	 * 2. Create and configure a sockaddr_in struct that listens
	 *      on the UDP port assigned to this->port_listen_.
	 * 
	 *    You probably want to use a bind address of INADDR_ANY,
	 *      so the socket can listen on all IP addresses for your computer.
	 * 
	 * 3. Create an actual UDP datagram internet socket.
	 * 
	 * 4. Throw a runtime error if the socket fails to create
	 * 
	 * 5. Bind the socket so the sockaddr_in struct's information
	 * 
	 * 6. Throw a runtime error if the socket fails to bind
	 * 
	 * 7. Remember the created socket as a member variable
	 */
	void Chat::initListeningSocket()
	{
		this->closeListeningSocket();

		sockaddr_in listen_address;
		std::memset(&listen_address, 0, sizeof(listen_address));

		listen_address.sin_family = AF_INET;
		listen_address.sin_addr.s_addr = INADDR_ANY;
		listen_address.sin_port = htons(this->port_listen_);

		int sock = socket(AF_INET, SOCK_DGRAM, 0);

		if (!this->isSocketValid(sock))
		{
			throw std::runtime_error("Failed  to create listening socket");
		}

		int bind_result = bind(sock, reinterpret_cast<sockaddr*>(&listen_address), sizeof(listen_address));

		if (bind_result < 0)
		{
			close(sock);
			throw std::runtime_error("Failed to bind listening socket");
		}

		this->socket_listen_ = sock;
		
	}
	
	/**
	 * Simply return the listening socket
	 */
	int Chat::getListeningSocket() const
	{
		return this->socket_listen_;
	}
	
	/**
	 * 
	 * Close the listening socket, if it's currently valid.
	 * 
	 * Probably want to rely on closeSocket() to avoid repeating code
	 * 
	 */
	void Chat::closeListeningSocket()
	{
		this->closeSocket(this->socket_listen_);
	}
	
	/**
	 * 
	 * Return true if the outbound socket is "valid"; false otherwise
	 * 
	 * Probably want to rely on isSocketValid() to avoid repeating code
	 * 
	 */
	bool Chat::isOutboundSocketValid() const
	{
		return this->isSocketValid(this->socket_out_);
	}
	
	/**
	 * Initialize/Create the outbound socket that will send data
	 *   to your chat parter.
	 * 
	 * You'll want to do the following:
	 * 
	 * 1. Call closeOutboundSocket(), just in case it's already valid
	 * 
	 * 2. Create a UDP datagram internet socket
	 * 
	 * 3. Throw a runtime error if the socket fails to create
	 * 
	 * 4. Remember the socket as a member variable
	 */
	void Chat::initOutboundSocket()
	{
		this->closeOutboundSocket();

		int sock = socket(AF_INET, SOCK_DGRAM, 0);

		if (!this->isSocketValid(sock))
		{
			throw std::runtime_error("Failed to create outbound socket");
		}

		this->socket_out_ = sock;
	}
	
	/**
	 * Simply return the outbound socket
	 */
	int Chat::getOutboundSocket() const
	{
		return this->socket_out_;
	}
	
	/**
	 * 
	 * Close the outbound socket, if it's currently valid.
	 * 
	 * Probably want to rely on closeSocket() to avoid repeating code
	 * 
	 */
	void Chat::closeOutboundSocket()
	{
		this->closeSocket(this->socket_out_);
	}
	
	/**
	 * Handle input that the user has entered
	 * 
	 * 1. If the user has entered the string "q" or "Q",
	 *      print the string "Sending quit!" and
	 *      call on this->quit()
	 * 
	 * 2. If the user has entered anything else,
	 *      send the input to a call to this->sendChatMessage()
	 */
	void Chat::handleUserInput(std::string input)
	{
		if (input == "q" || input =="Q")
		{
			std::cout << std::endl << "Sending quit!" << std::endl;
			this->quit();
			return;
		}

		this->sendChatMessage(input);		
	}
	
	/**
	 * Send a chat message!
	 * 
	 * 1. Return immediately if the message is empty
	 * 
	 * 2. Throw a runtime error if the outbound isn't valid
	 * 
	 * 3. Create a sockaddr_in struct and fill it with data pointing to
	 * 	the outbound hostname and outgoing port
	 * 
	 * 4. Send the data with sendto()
	 * 
	 * 5. If no bytes were sent, throw a runtime error
	 * 
	 * 6. Otherwise you're done
	 * 
	 * You would normally want to send your data in a loop,
	 * 	piece by piece (checking the return value of sendto) until
	 *  its done. But for the purposes of this project, we can
	 * 	probably just assume one call to sendto() ends up sending
	 * 	the entire chat message.
	 */
	void Chat::sendChatMessage(std::string message)
	{
		if (message.empty())
		{
			return;
		}

		if (!this->isOutboundSocketValid())
		{
			throw std::runtime_error("Outbound socket is not valid");
		}

		sockaddr_in dest_address;
		std::memset(&dest_address, 0, sizeof(dest_address));

		dest_address.sin_family = AF_INET;
		dest_address.sin_port = htons(this->port_out_);

		hostent* host = gethostbyname(this->hostname_.c_str());

		if (host == NULL)
		{
			throw std::runtime_error("Failed to resolve hostname");
		}

		std::memcpy(&dest_address.sin_addr, host->h_addr, host->h_length);

		ssize_t bytes_sent = sendto(this->socket_out_, message.c_str(), message.length() + 1, 0,
			       	reinterpret_cast<sockaddr*>(&dest_address), sizeof(dest_address));

		if (bytes_sent <= 0)
		{
			throw std::runtime_error("Failed to send chat message");
		}	
	}
	
	/**
	 * Receive a chat message
	 * 
	 * Start by throwing a runtime error if the listening socket isn't valid
	 * 
	 * In this program, each chat message should end with a null terminator.
	 * It is possible that one call to recv() might only give you part of a messsage,
	 * 	and not have a null terminator at the end. Thus, you need to figure out
	 * 	a way to call recv() in a loop, and concatenate characters received
	 * 	until you see the null terminator.
	 * Once you see the null terminator, return the entire concatenated string.
	 * 
	 * Here is an idea:
	 * 
	 * 	1. Start with an empty string to represent the message you have received
	 * 
	 *  2. Enter into a while loop
	 * 
	 *  2a. Try to receive some data from the listening socket
	 * 
	 * 	2b. If the received length is -1, pretend you have received "SOCKETERROR" and break the loop
	 * 
	 * 	2c. If the data you have received doesn't contain a null terminator at the end,
	 * 		add one artificially so you can concatenate it onto your message string
	 * 
	 * 	2d. If the data you received contains a null terminator at the end,
	 * 		this is the end of the current chat message. Add the piece you've just
	 * 		received to the message string and break the loop.
	 * 
	 *  4. Return the message you have received.
	 * 
	 * Note that this function uses a call to recv() with no
	 * 	consideration for timing out or select(), which means the
	 * 	thread could get stuck forever if your partner were to disconnect.
	 * 
	 * This is why - when someone ends the chat session - we have BOTH
	 * 	partners send the quit sequence.
	 */
	std::string Chat::receiveChatMessage()
	{
		if (!this->isListeningSocketValid())
		{
			throw std::runtime_error("Listening socket is not valid");
		}

		std::string message = "";
		char buffer[Chat::BUFFER_SIZE];

		while (true)
		{
			std::memset(buffer, 0, Chat::BUFFER_SIZE);

			ssize_t received_length = recv(this->socket_listen_, buffer, Chat::BUFFER_SIZE - 1, 0);

			if (received_length < 0)
			{
				return "SOCKETERROR";
			}

			bool found_null = false;

			for (ssize_t i = 0; i < received_length; i++)
			{
				if (buffer[i] == '\0')
				{
					found_null = true;
					break;
				}
			}
			message += buffer;

			if (found_null)
			{
				break;
			}
		}

		return message;
	}
	
	/**
	 * Do whatever needs to be done based on the chat message
	 * 	you have just received from your chat partner
	 * 
	 * If you've detected that the Chat::QUIT_SEQUENCE appears
	 * 	in *any part* of the received chat message,
	 *  print "*** Buddy has disconnected ***" on a line,
	 * 	then print "(press enter to exit if stuck)" on
	 *  another line so the user knows how to break
	 * 	the std::getline() call. Then, call this->quit() to
	 * 	perform quitstuffs
	 * 
	 * Otherwise, print "Buddy > " and then the received
	 * 	chat message.
	 */
	void Chat::handleReceivedChatMessage(std::string message)
	{
		if (message.find(Chat::QUIT_SEQUENCE) != std::string::npos)
		{
			std::cout << std::endl;
			std::cout << "*** Buddy has disconnected ***" << std::endl;
			std::cout << "(press enter to exit if stuck)" << std::endl;

			this->quit();
			return;
		}

		std::cout << std::endl;
		std::cout << "Buddy > " << message << std::endl;
	}
	
	/**
	 * Properly quit the chat session
	 * 
	 * 1. Set this->quitting_ to true, so loops elsewhere know to break
	 * 		(don't worry about race conditions; this app is simple)
	 * 
	 * 2. Send Chat::QUIT_SEQUENCE as a message to your partner
	 * 		THREE TIMES, so they have a good chance to exit their loop, too.
	 */
	void Chat::quit()
	{
		this->quitting_ = true;

		this->sendChatMessage(Chat::QUIT_SEQUENCE);
		this->sendChatMessage(Chat::QUIT_SEQUENCE);
		this->sendChatMessage(Chat::QUIT_SEQUENCE);
	}
	
	///	Return true if the chat app wants to quit; False otherwise
	bool Chat::isQuitting()
	{
		return this->quitting_;
	}
	
	/**
	 * Return true if the socket is valid, false otherwise
	 * 
	 * The most proper way to detect a valid socket
	 * 	is by calling getsockopt() and checking the return value.
	 * 
	 * However, for simplicity, we can just say
	 * 	that a "valid" socket will have an integer value of at least 1
	 */
	bool Chat::isSocketValid(int sock) const
	{
		return sock >= 1;
	}
	
	/**
	 * Close a socket
	 * 
	 * Notice this takes a socket by REFERENCE
	 * 
	 * This means you can call on it to close either of your sockets
	 * 	and expect it will properly modify the correct member variable
	 * 
	 * 1. If the socket isn't valid, return immediately
	 * 
	 * 2. close() the socket
	 * 
	 * 3. Set the socket's value to Chat::INVALID_SOCKET,
	 * 		which is just -1
	 */
	void Chat::closeSocket(int& sock)
	{
		if (!this->isSocketValid(sock))
		{
			return;
		}

		close(sock);
		sock = Chat::INVALID_SOCKET;
	}
	
	/**
	 * Spawn the sender and receiver threads.
	 * 
	 * The sending thread should enter into c_senderThread(),
	 * 	which in turn will call the current instance's Chat::senderThread()
	 * 
	 * The receiving thread should enter into c_receiverThread(),
	 * 	which in turn will call the current instance's Chat::receiverThread()
	 * 
	 * You'll also want to remember thread IDs with member variables.
	 * 
	 * Don't forget to check for errors, and throw when unsuccessful
	 */
	void Chat::spawnThreads()
	{
		int sender_result = pthread_create(&this->tid_sender_, NULL, c_senderThread, this);

		if (sender_result !=0)
		{
			throw std::runtime_error("Failed to create sender thread");
		}

		int receiver_result = pthread_create(&this->tid_receiver_, NULL, c_receiverThread, this);

		if (receiver_result != 0)
		{
			throw std::runtime_error("Faile to create receiver thread");
		}
		
	}
	
	/**
	 * senderThread() is just a loop that calls on two other
	 * 	functions to handle user input.
	 * 
	 * 1. Enter into a loop.
	 * 
	 * 2. Grab an std::string of the user's input by calling
	 *      on the member function getUserInput().
	 * 
	 * 3. Call on the member function handleUserInput()
	 *      to actually handle the user's input.
	 * 
	 * 4. Break the loop if the member variable this->quitting_ is true.
	 * 
	 * 5. Otherwise, keep looping.
	 * 
	 */
	void Chat::senderThread()
	{
		while (!this->isQuitting())
		{
			std::string input = this->getUserInput();
			this->handleUserInput(input);

			if (this->isQuitting())
			{
				break;
			}
		}
	}
	
	/**
	 * receiverThread() is just a loop that continually tries to receive
	 * data from your chat partner.
	 * 
	 * 1. Enter into a loop.
	 * 
	 * 2. Call this->receiveChatMessage() to get a string from
	 *      your chat partner.
	 * 
	 * 3. Call on this->handleReceivedChatMessage() with the string.
	 * 
	 * 4. Break the loop if the member variable this->quitting_ is true.
	 * 
	 * 5. Otherwise, keep looping.
	 * 
	 */
	void Chat::receiverThread()
	{
		while (!this->isQuitting())
		{
			std::string message = this->receiveChatMessage();
			this->handleReceivedChatMessage(message);

			if (this->isQuitting())
			{
				break;
			}
		}
	}
	
	/**
	 * Get user input from the keyboard and return as an std::string
	 * 
	 * 1. Prompt the user for input with:
	 *    "Enter message (or q to quit) > "
	 * 
	 * 2. Use std::getline (or similar) to fetch an entire line
	 *      of user input.
	 * 
	 * 3. Return the line as an std::string
	 */
	std::string Chat::getUserInput()
	{
		std::string input;

		std::cout << "Enter message (or q to quit) > ";
		std::getline(std::cin, input);

		return input;
	}
	
	/**
	 * This is where you should join the two threads spawned by
	 * your program: The sender thread and the receiver thread.
	 * 
	 * Remember, you should be able to find thread IDs in member variables.
	 * 
	 * Don't forget to check for errors.
	 * 
	 * Do not join threads elsewhere in this program.
	 */
	void Chat::joinThreads()
	{
		int sender_result = pthread_join(this->tid_sender_, NULL);

		if (sender_result != 0)
		{
			throw std::runtime_error("Faile to join sender thread");
		}

		int receiver_result = pthread_join(this->tid_receiver_, NULL);

		if (receiver_result != 0)
		{
			throw std::runtime_error("Faile to join receiver thread");
		}
	}
}












