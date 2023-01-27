

# webserv
This project is a configurable HTTP webserver made with C++ inspired by NGINX. It serves as an objective to me for learning socket programming and HTTP RFC Protocol. 

![demo](https://github.com/neosizzle/webserv/blob/main/webserv.gif)

## Installation
`git clone https://github.com/neosizzle/webserv.git`
`make && ./webserv [config file]`

## Features 
 - HTTP 1.1 RFC Compliant (Correct headers)
 - Compatible with web browsers
 - Default error pages
 - Custom CGI supported
 - Able to serve static sites
 - Able to upload files
 - Able to configure multiple ports and hosts
 - Able to configure HTTP redirection
 - Able to configure static site location
 - Able to configure CGI (extensions, locations, number of CGI)
 - Able to handle chunked requests
 - Non-blocking between requests

## Writeup
Here is a high level overview of the entire communication flow represented in a sequence UML diagram

```mermaid
sequenceDiagram

Client ->> select() : connection handshake (tcp)
activate select()
loop  x seconds
select()->Server code: waiting for accepting connection..
end
Server code -->> select() : Connection accepted
select() -->> Client : handshake confirmed (tcp)
deactivate select()

loop  x seconds
Client-->select(): waiting for data...
end

activate select()
Client ->> select() : request (http)
select()->>Server code: Request carried  
Server code ->> fs / CGI : Unchunk and parses request 
fs / CGI -->> Server code : Returns primitive value / data
Server code -->> select() : parses response
deactivate select()
loop  x seconds
Client-->select(): waiting for write avail...
end

activate select()
select() -->> Client : response (http)
deactivate select()
```

### Communications across networks
When it comes to communications across different entities (processes , networks, devices), we will often need a way to carry data from one point to the other.

For communication between processes, we have used things like pipes to talk from one process to another. The implementation of pipes varies across Operating systems, but all of them are often integrated into the operating system file I/O subsystem.

In C, we can access the ends of the pipes using numeric file descriptors. When we write to the read end of the pipe, it would simple transmit data to the write end of the pipe. In one direction only.

We can also achieve communication of different devices in a network using sockets. It is very similar to pipes :
 - Data written from one end can be read from another
 - Are represented by file descriptors on your C programs
 - Implementation dependent of the Operating System

However, do note that unlike pipes, sockets are **bi-directional** (read and write from both ends) and they transfer data using the **TCP protocol**. Which breaks down the data into packets and then reassembles them in the destination to mitigate data loss issue when transferring streams of data over long distances.

The server and client have their own seperate sockets they can use via fd. And the client socket will `connect()`, the server socket will `listen()` and `accept()` incoming connections.

Whatever we write to the clients connected socket will be readable at the servers accepted socket.

You can build a simple TCP server using the following code
```c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(void)
{
    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");
    
    // Listen for clients:
    if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections on port 2000 .....\n");
    
    while (1)
	{
		// Accept an incoming connection:
		client_size = sizeof(client_addr);
		client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
		
		if (client_sock < 0){
			printf("Can't accept\n");
			return -1;
		}
		printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		
		// Receive client's message:
		if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
			printf("Couldn't receive\n");
			return -1;
		}
		printf("Msg from client: %s\n", client_message);
		
		// Respond to client:
		strcpy(server_message, "This is the server's message.");
		
		if (send(client_sock, server_message, strlen(server_message), 0) < 0){
			printf("Can't send\n");
			return -1;
		}
		
		// Closing the socket:
		close(client_sock);
	}
	
    close(socket_desc);
    
    return 0;
}
```

on another terminal, you can use `netcat` as a client. `nc localhost -v 2000`

### select() and blocking fds.
suppose we have the following bash script : 
`cat | sort`

If we run this, we are stuck with a `cat` prompt and it will only read data until we hit **ctrl-D** to send a EOF and proceed with sort.

If we didnt hit **ctrl-D** , then the sort command will not run.


This can happen to sockets as well. Imagine the client application as the `cat` program and the server application as the `sort` program. The server will hang forever, unable to accept new connections if the previous connected client havent supplied the EOF yet (disconnect).

In sockets, when you try to read from a socket which has no data being sent, it **hangs**. For example, if you have 2 `netcat` sessions on the server code above, the second one is unable to communicate to the server if the first one does not send anything. 

To mitigate this behavior, we use a the `select()` function.

Here is its declaration; 
```c
int select(int _nfds_, fd_set *restrict _readfds_,
                  fd_set *restrict _writefds_, fd_set *restrict _exceptfds_,
                  struct timeval *restrict _timeout_);
```
When we call `select()` it will **block** until any one of the many fds that is specified to be unlocked **OR** a timeout had been reached. That way, we dont need wait for the clients to queue up for reading data. 

To implement it in the TCP server above, make the following changes;
```c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
	fd_set rset;
    char server_message[2000], client_message[2000];
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully, FD %d\n", socket_desc);
    
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");
    
    // Listen for clients:
    if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections on port 2000 .....\n");
    
	// clear the descriptor set
    FD_ZERO(&rset);
    FD_SET(socket_desc, &rset);

    int *client_fds = malloc(100 * sizeof(int));
    int max_client = 0;
    int num_avail_fds;
    struct timeval	timeout;

    num_avail_fds = 0;
    
    while (1)
	{
        fd_set			read_fds_copy;

		// while num of avail fds is not 1, keep selecting
        while (!num_avail_fds)
        {
            timeout.tv_sec  = 2;
			timeout.tv_usec = 0;
			memcpy(&read_fds_copy, &rset, sizeof(rset));
            num_avail_fds =  select(6, &read_fds_copy, NULL, NULL, &timeout);
        }
        
        printf("FD_ISSET(3, &read_fds_copy) %d\n",FD_ISSET(3, &read_fds_copy));
        printf("FD_ISSET(4, &read_fds_copy) %d\n",FD_ISSET(4, &read_fds_copy));
        printf("FD_ISSET(5, &read_fds_copy) %d\n",FD_ISSET(5, &read_fds_copy));

        // handle connection
        if (FD_ISSET(socket_desc, &read_fds_copy))
        {
		    // accept connection and set fd to be read
            client_size = sizeof(client_addr);
		    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

      		if (client_sock < 0){
                printf("Can't accept\n");
                continue;
		    }
            FD_SET(client_sock, &rset);
            num_avail_fds = 0;
            client_fds[max_client++] = client_sock;
		    printf("Client connected at IP: %s and port: %i. FD %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_sock);
            continue;    
        }

        // handle read
        for (size_t i = 0; i < max_client; i++)
        {
            if (FD_ISSET(client_fds[i], &read_fds_copy))
            {
                printf("handling read from fd %d\n", client_fds[i]);
                // Receive client's message:
                if (recv(client_fds[i], client_message, sizeof(client_message), 0) < 0){
                    printf("Couldn't receive\n");
                    return -1;
                }
                printf("Msg from client: %s\n", client_message);

                // after read, write then close connection and remove from fdset
                // Respond to client:
                strcpy(server_message, "This is the server's message.");
                
                if (send(client_fds[i], server_message, strlen(server_message), 0) < 0){
                    printf("Can't send\n");
                    return -1;
                }

                // Closing the socket:
		        close(client_fds[i]);

                int fd_save = client_fds[i];
                // move the sockets back in our client_fds
                for (size_t j = i; j < max_client - 1; j++)
                    client_fds[j] = client_fds[j + 1];
                FD_CLR(fd_save, &rset);
                client_fds[max_client--] = -1;
                num_avail_fds = 0;
                break;
            }
        }
		
	}
	
    // closing server socket
    close(socket_desc);
    
    return 0;
}
```

If implemented correctly, the 2 `netcat` sessions wont block when the first one hangs.

In the example above, we made a change where we keep track of multiple clients with an array. We also used `select()` before any and `FD_ISSET()` before reading form clients. 

The `select()` call made sure that our read / write code wont run until the we have available file descriptor(s) and `FD_ISSET()` helps us to pinpoint which file descriptor is available to read / write from.

### HTTP 1.1 RFC
Now that we know how to send plaintext data through sockets, we have to format the data so that it complies with the [Http RFC](https://www.rfc-editor.org/rfc/rfc2616#section-4.2) which is a set of rules and conventions about the web protocol. 

Generally, a sample request would look like this
```
GET / HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
Accept-Language: en-GB,en;q=0.5
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
```

The first line is a **request line**, consisting of the case-sensitive request method, a [space](https://en.wikipedia.org/wiki/Space_(punctuation) "Space (punctuation)"), the requested URL, another space, the protocol version, a [carriage return](https://en.wikipedia.org/wiki/Carriage_return "Carriage return"), and a [line feed](https://en.wikipedia.org/wiki/Line_feed)

The following lines are headers, which provide more context about the request and the client. For example, the `User-Agent`  header provides the web client information. In this case, the requester is using Mozilla firefox browser. Other headers carry their own definition and importance, and can be referred to the RFC.

Once a program receives this string of text using a socket, it will parse and process this request and it would generate a plaintext response like so: 
```
HTTP/1.1 200 OK
Date: Mon, 23 May 2005 22:38:34 GMT
Content-Type: text/html; charset=UTF-8
Content-Length: 155
Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT
Server: Apache/1.3.3.7 (Unix) (Red-Hat/Linux)
ETag: "3f80f-1b6-3e1cb03b"
Accept-Ranges: bytes
Connection: close

<html>
  <head>
    <title>An Example Page</title>
  </head>
  <body>
    <p>Hello World, this is a very simple HTML document.</p>
  </body>
</html>
```

The first line of a HTTP Response is a a **status line**, consisting of the protocol version, a [space](https://en.wikipedia.org/wiki/Space_(punctuation) "Space (punctuation)"), the [response status code](https://en.wikipedia.org/wiki/List_of_HTTP_status_codes "List of HTTP status codes"), another space, a possibly empty reason phrase, a [carriage return](https://en.wikipedia.org/wiki/Carriage_return "Carriage return") and a [line feed](https://en.wikipedia.org/wiki/Line_feed "Line feed").

It has headers defined below which also carries its own meanings. However, one special thing is particular is the block of text below the headers. That is the response body. The actual message the client wants to receive upon requesting for resources.

The body can be anything, HTML, JSON etc.. but as far as HTTP is concerned, they are all generated as plain text and sent back to the client via the sockets.

### CGI
So far, all the requests that we receive are processed via the server; as in the server has code which parses, interprets and sends back a response. The server can also call external programs to parse and interprets the request for us, and we respond the client with the programs output. This program is called a [CGI (Common gateway interface)](https://en.wikipedia.org/wiki/Common_Gateway_Interface)

Obviously, the program has to comply with the [RFC](https://www.rfc-editor.org/rfc/rfc3875.html) as well. But for the sake of simplicity, we will omit it for the writeup.

Say you already have a CGI set up at your server. Your client can request something like this
```
POST /runpy HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0
// other headers.....


print(60 + 9)
```

Your server can detect the route `/runpy` and call `fork()` on a python interpreter on the servers machine. The interpreter then takes the request body as STDIN (`print(60 + 9)`) and executes it. Keep in mind that the interpreter can be any program. PHP, Perl, Ruby, JS etc are all valid programs to be CGI's.

Once the python program finishes, you can `pipe()` the output to your server process and include it in the response text :

```
HTTP/1.1 200 OK 
Date: Mon, 23 May 2005 22:38:34 GMT 
Content-Type: text/html; charset=UTF-8 
// other headers....


69
```

And there, you have just used a CGI to process a user request.