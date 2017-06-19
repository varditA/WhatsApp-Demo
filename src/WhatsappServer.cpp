//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include "WhatsappServer.h"

WhatsappServer::WhatsappServer() {
    FD_ZERO(&openedSockets);
}

WhatsappServer::~WhatsappServer() {
    FD_ZERO(&rfds);
    close(socketId);
}

/* ------------------------ Private Functions ------------------------------------------ */

/** open the connection to the server
 * @return 0 if the connection failed.
 *  if it has succeed, returns socket's id
 */
int WhatsappServer::setConnection(int portNum) {
    if (setHostName() != 0 or setHostent() != 0) {
        return -1;
    }

    /* todo put in createSocket ? */
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = this.hp->h_addrtype;
    memcpy(&sa.sin_addr, this.hp->h_addr, this.hp->h_length);
    sa.sin_port= htons(portNum);

    if (createSocket() != 0)
    {
        return -1;
    }

    if (bind(socketId , (struct sockaddr *)&sa , sizeof(struct sockaddr_in)) < 0) {
        close(socketId);

        /*todo error*/
        return(-1);
    }

    socketAdd = sizeof(is);
    listen(socketId, 10); /* max # of queued connects */
    return 0;
}

/**
 * setting the host name
 * @return 0 if succeed and -1 if not.
 */
int WhatsappServer::setHostName() {
    if (gethostname(this.myName, MAX_HOSTNAME_LENGTH) != 0)
    {
        /* todo error */
        return -1;
    }
    return 0;
}

/**
 * setting a structure of type hostent for the given host name
 * @return 0 if succeed and -1 if not
 */
int WhatsappServer::setHostent() {
    this.hp = gethostbyname(this.myName);
    if (this.hp == NULL)
    {
        /* todo error */
        return -1;
    }
    return 0;
}

int WhatsappServer::createSocket() {
    if (socketId= socket(AF_INET, SOCK_STREAM, 0) < 0)
        /* todo error */
        return(-1);
    FD_SET(socketId, &openedSockets);
    return 0;
}

/**
 * The function waits for new client. For every new client, it creats
 * new socket and add it to the server's clients.
 * @return
 */
int WhatsappServer::waitForClients() {
    int maxId = socketId;  /* the max id of socket in the openedSockets' set */
    while (true)
    {
        /* check which socket has been activated */
        int usingSelect = select(maxId + 1, &openedSockets, NULL, NULL, NULL);
        if (usingSelect < 0 && errno != EINTR)
        {
            /* todo error */
        }

        /* the main socket has been called, else : one of the other sockets*/
        if (FD_ISSET(socketId, &openedSockets))
        {
            int newSocketNum = getConnection();
            if (newSocketNum != 0)
            {
                FD_SET(newSocketNum, &openedSockets);
                /* todo what to do ? */
                string name = "";   /* todo fix it */
                string message = "client1 connected."; /* todo fix it */

                WhatsappClient newClient = new WhatsappClient(name, this, newSocketNum);
                clients.push_back(newClient);
                if (send(newSocketNum, message, strlen(message), 0) != strlen(message))
                {
                    /* todo error */
                }
            }
        } else
        {
            for (WhatsappClient client: clients)
            {
                int clientSocket = client.getSocketId();
                if (FD_ISSET(clientSocket, &openedSockets))
                {
                    /* read and write */
                }
            }
        }

        /* todo remove socket from openedsockets using FD_CLR */
    }
}

/**
 * open a new socket for a new client
 * @return the socket's num if succeed and 0 otherwise
 */
int WhatsappServer::getConnection() {
    /* the server sets a new socket that will be used only for a specific client */
    int clientSocket;
    clientSocket = accept(socketId, NULL, NULL);
    if (clientSocket < 0)
    {
        /* todo error */
        return -1;
    }
    return clientSocket;
}



/* ------------------------ Public Functions ------------------------------------------ */
void WhatsappServer::create_group(string groupName, string callerName, vector<string> members){}

void WhatsappServer::sendMsg(string senderName, string receiverName, string msg){}

/**
 * Sends to the caller a list (might be empty) of currently connected
 * client names (alphabetically order), separated by comma without spaces.
 * @param senderName
 */
void WhatsappServer::who(string caller){}

void WhatsappServer::exit(string caller){}