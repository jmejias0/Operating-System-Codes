# Homework - UDP Socket Chat

My submission for Computer Science CPSC 351, Section 07, extra credit assignment

# My Information

* name: Jose Mejias
* CWID: 820507572
* Email: jmejias0@csu.fullerton.edu

# Instructions

I will divide this extra credit assignment in 9 steps, only modifying Chat.hpp and Chat.cpp

1. Understand the program arguments

Read three command-line arguments: the local port, the remote hostname and the remote destination port

2. Design the chat class data

Sort the local, remote hostname, remote port, sending socket, receiving socket, remote address information, and the share quitting flag

3. Create UDP sending socket

Use socket(AF_INET, SOCK_DGRAM, 0) and to create a UDP socket that will send messages to the chat partner

4. Create and bind the UDP receiving socket

Use socket(AF_INET, SOCK_DGRAM, 0) and bind(...) so the program can listen for incoming UDP messages in the local port

5. Build the sender thread function

Repeatedly ask the user for input, send normal messages with a null terminator, and send the quit sequences three times when the user enters q or Q

6. Build the receiver thread function

Repeatedly call recv(...), collect message fragments, detect null terminators, print complete messages, and handle the quit sequence when received

7. Add thread startup and joining logic

Start one sender thread and one receiver thread, and then have the main chat logic wait for both threads using join()

8. Add clean shotdown behavior

Set Chat::quitting_ to true when either side quits, send the quit sequence when needed, close sockets, and make sure both threads can exit.

9. Test with simple tools

First test sending and receiving with netcat, then test two instances of your program using the reverse ports
