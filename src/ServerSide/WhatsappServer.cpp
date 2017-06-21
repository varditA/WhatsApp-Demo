//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/timeb.h>
#include "WhatsappServer.h"

/* ---------------------------- Main -------------------------------------------------- */

int main(int argc, char * argv[]) {
    if (argc !=  2)
    {
        /* todo error */
        return -1;
    }

    try{
        const int PORT_NUM = std::atoi(argv[1]);
        WhatsappServer * server = new WhatsappServer();
        if (server->activateServer(PORT_NUM) < 0)
        {
            /* todo error*/
            return 1;
        }
        server->waitForConnection();
    }
    catch (exception e){
        /*todo error */
        cerr << "error";
        return -1;
    }

    return 0;
}

/* ------------------------ Public Functions ------------------------------------------ */



WhatsappServer::WhatsappServer() {
    myName = new char(MAX_HOSTNAME_LENGTH);
    FD_ZERO(&openedSockets);
}

WhatsappServer::~WhatsappServer() {
    delete myName;
    FD_ZERO(&openedSockets);
//    close(socketId);
}

/** open the connection to the server
 * @return 0 if the connection failed.
 *  if it has succeed, returns socket's id
 */
int WhatsappServer::activateServer(int portNum) {
    if (setHostName() != 0 or setHostent() != 0) {
        return -1;
    }

    /* todo put in createSocket ? */
    sa.sin_family = hp->h_addrtype;
    sa.sin_port= htons(portNum);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);


    if (createSocket() != 0)
    {
        return -1;
    }
    if (bind(socketId , (struct sockaddr *)&sa , sizeof(struct sockaddr_in)) < 0) {
        close(socketId);

        /*todo error*/
        return(-1);
    }

    listen(socketId, 10); /* max # of queued connects */
    cout << "whatsappServer " <<  portNum << "\n" << flush;
    return 0;
}


/**
 * The function waits for new client. For every new client, it creats
 * new socket and add it to the server's clients.
 * @return
 */
int WhatsappServer::waitForConnection() {
    maxId = socketId;  /* the max id of socket in the openedSockets' set */
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
            addNewClient();
        } else
        {
            getMsgFromClient();
        }

        /* todo remove socket from openedsockets using FD_CLR */
    }
}


/* ------------------------ Private Functions ------------------------------------------ */

/**
 * setting the host name
 * @return 0 if succeed and -1 if not.
 */
int WhatsappServer::setHostName() {
    if (gethostname(myName, MAX_HOSTNAME_LENGTH) != 0)
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
    hp = gethostbyname(myName);
    if (hp == NULL)
    {
        /* todo error */
        return -1;
    }
    return 0;
}

int WhatsappServer::createSocket() {
    auto on = 1;
    socketId= socket(AF_INET,SOCK_STREAM,0);

    if (socketId < 0) {
        /* todo error */
        return(-1);
    }
    if (setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) < 0)
    {
        /* todo error */
        return -1;
    }
    FD_SET(socketId, &openedSockets);
    return 0;
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

int WhatsappServer::isClientExist(string name) {
    if (clients.find(name) != clients.end())
    {
        return 0;
    }
    return -1;
}

int WhatsappServer::isGroupExist(string name) {
    if (groups.find(name) != groups.end())
    {
        return 0;
    }
    return -1;
}


/* ------------------------ Currently Working On ------------------------------------------ */


void WhatsappServer::addNewClient()  {
    int newSocketNum = getConnection();
    if (newSocketNum == 0) {
        return;
    }

    FD_SET(newSocketNum, &openedSockets);
    maxId = newSocketNum;

    char buffer[TOTAL_MSG_LENGTH] = ""; /*TODO like that everywhere */


    if (readMsg(newSocketNum,buffer) < 0)
    {
        /* can't get the name*/
        return;
    }

    string name(buffer);

    if (isClientExist(name)== 0 || isGroupExist(name) == 0)
    {
        if (send(newSocketNum, CONNECT_FAILURE_MSG, strlen(CONNECT_FAILURE_MSG), 0) != strlen(CONNECT_FAILURE_MSG))
        {
            /* todo error */
            return;
        }
    }

    ClientInfo * client = new ClientInfo(newSocketNum,name);
    clients.insert(make_pair(name,client));

    if (send(newSocketNum, CONNECT_SUCCESS_MSG, strlen(CONNECT_SUCCESS_MSG), 0) != strlen(CONNECT_SUCCESS_MSG))
    {
        /* todo error */
        return;
    } else
    {
        string message = string(name) + string(" connected."); /* todo fix it */
        cout << message << endl << flush;
    }

}

void WhatsappServer::getMsgFromClient() {
            for (auto& client: clients)
            {
                int clientSocket = client.second->getSocketId();
                if (FD_ISSET(clientSocket, &openedSockets))
                {
                    char message[TOTAL_MSG_LENGTH] = "";
//                    if(recv(newSocketNum, message , 30 , 0) < 0)
//                    {
//                        /* todo error */
//                    }
                    readMsg(clientSocket,message);
                    cout << message << endl << flush;
//                    handleMsg(client, message);
                }
            }
}


int WhatsappServer::readMsg(int socketNum, char *buffer) {
    char * temp = new char[3];
    char * pointerBuffer = temp;

    int bcount = 0;       /* counts bytes read */
    int br = 0;           /* bytes read this pass */
    int found = 1;

    while (bcount < 3)
    {
        br = read(socketNum, temp, 3-bcount);

        if (br >= 0)
        {
            bcount += br;
            temp += br;
        }
        else
        {
            return -1;
        }
    }

    int msgLength = atoi(pointerBuffer);
//    delete [] temp;           /* todo delete */

    bcount = 0;

    while (bcount < msgLength) { /* loop until full buffer */
        br = read(socketNum, buffer, msgLength-bcount);
        if (br >= 0){
            bcount += br;
            buffer += br;
        } else {
            return -1;
        }
    }

    return bcount;
}




/* ------------------------ In Progress Functions ------------------------------------------ */

void WhatsappServer::create_group(string groupName, string callerName,
                                  vector<string> members){
    if (isClientExist(groupName)== 0 || isGroupExist(groupName) == 0)
    {
        /* todo error */
        return;
    }

    /* todo fix set */
    set<string> names;
    set<ClientInfo*> clientsInGroup;
    map<string,ClientInfo*>::iterator it;

    it = clients.find(callerName);
    if (it == clients.end())
    {
        return;
    }
    names.insert(callerName);

    it = clients.begin();

    for (string member: members)
    {
        auto client = clients.find(member);
        if (client != clients.end())
        {
            if (names.find(member) != names.end())
            {
                names.insert(it->first);
                clientsInGroup.insert(it->second);

            }
        }
    }

    Group * group = new Group(groupName,clientsInGroup);
    groups.insert(make_pair(groupName,group));
}

void WhatsappServer::sendMsg(string senderName, string receiverName, string msg){}

/**
 * Sends to the caller a list (might be empty) of currently connected
 * client names (alphabetically order), separated by comma without spaces.
 * @param senderName
 */
void WhatsappServer::who(string caller){
    string msg("");
    bool first = true;

    /* todo sort ? */


    map<string,ClientInfo*>::iterator it;
    it = clients.find(caller);
    if (it == clients.end())
    {
        return;
    }

    it = clients.begin();

    while (it != clients.end())
    {
            if (!first)
            {
                msg += ",";

            } else
            {
                first = false;
            }
            msg += it->first;
            ++it;

    }

    /* add the length to the msg's beginning */
    if (send(it->second->getSocketId(), msg.c_str(), strlen(msg.c_str()),0) !=
        strlen(msg.c_str()))
    {
        /* todo error */
    }
}

void WhatsappServer::exit(string caller){

}