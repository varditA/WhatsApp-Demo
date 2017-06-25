//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include <stdio.h>
#include <stdlib.h>
//#include <errno.h>
//#include <strings.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/time.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <sys/timeb.h>
#include "WhatsappServer.h"

/* ---------------------------- Main --------------------------------------- */

int main(int argc, char * argv[]) {
    if (argc !=  2)
    {
        /* todo error */
        return -1;
    }

    try{
        const int PORT_NUM = std::atoi(argv[1]);
        WhatsappServer server;
        if (server.activateServer(PORT_NUM) < 0)
        {
            /* todo error*/
            return 1;
        }
        server.waitForConnection();
//        delete (server);
    }
    catch (exception e){
        /*todo error */
        cerr << "error";
        return -1;
    }


    return 0;
}

/* ------------------------ Public Functions ------------------------------- */



WhatsappServer::WhatsappServer() {
    myName = new char(MAX_HOSTNAME_LENGTH);
}

WhatsappServer::~WhatsappServer() {
    delete myName;
    FD_ZERO(&openedSockets);
    close(socketId);
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
    if (bind(socketId, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0)
    {
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
        FD_ZERO(&openedSockets);
        FD_SET(0, &openedSockets);
        FD_SET(socketId, &openedSockets);
        for (auto& client: clients)
        {
            int socket = client.second->getSocketId();
            FD_SET(socket, &openedSockets);

        }

        int usingSelect = select(maxId + 1, &openedSockets, NULL, NULL, NULL);

        if (usingSelect < 0 && errno != EINTR)
        {
            /* todo error */
        }

        /* one of the other sockets has been activated*/
        if (FD_ISSET(socketId, &openedSockets))
        {
            addNewClient();

        } else if (FD_ISSET(0, &openedSockets))
        {
            shutDownServer();

        } else
        {
            getMsgFromClient();
        }

        FD_CLR(0, &openedSockets);
        FD_CLR(socketId, &openedSockets);
        for (auto& client: clients)
        {
            int socket = client.second->getSocketId();
            FD_CLR(socket, &openedSockets);
        }
    }
}


/* ------------- Setting The Server -------------- */

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


/* ----------- Operating The Server -------------- */


void WhatsappServer::addNewClient()  {
    int newSocketNum = getConnection();
    if (newSocketNum == 0) {
        return;
    }

    FD_SET(newSocketNum, &openedSockets);

    if (newSocketNum > maxId)
    {
        maxId = newSocketNum;
    }

    char buffer[TOTAL_MSG_LENGTH] = ""; /*TODO like that everywhere */


    if (readMsg(newSocketNum,buffer) < 0)
    {
        /* can't get the name*/
        /* todo error*/
        return;
    }

    string name(buffer);

    if (isClientExist(name)== 0 || isGroupExist(name) == 0)
    {

        string msgToSend = setMsgLength(CONNECT_FAILURE_MSG) +
                CONNECT_FAILURE_MSG;

        if (writeMsg(newSocketNum,msgToSend.c_str()) < 0)
        {
            /* todo error */
        }
        return;
//        if (send(newSocketNum, msgToSend.c_str(),
//                 strlen(msgToSend.c_str()), 0) != strlen(msgToSend.c_str()))
//        {
//            /* todo error */
//            return;
//        }
    }

    ClientInfo * client = new ClientInfo(newSocketNum,name);
    clients.insert(make_pair(name,client));

    string msgToSend = setMsgLength(CONNECT_SUCCESS_MSG) + CONNECT_SUCCESS_MSG; /* todo check */

    if (writeMsg(newSocketNum, msgToSend.c_str()) < 0)
    {
        /* todo error */
        return;
    }
//    if (send(newSocketNum, msgToSend.c_str(),
//             strlen(msgToSend.c_str()), 0) != strlen(msgToSend.c_str()))
//    {
//        /* todo error */
//        return;
//    } else
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
            if (readMsg(clientSocket,message) < 0)
            {
                /* todo error */
            }

            string strMsg(message);
            /* todo delete */
            excCommand(strMsg, client.second);

        }
    }
}

void WhatsappServer::shutDownServer() {
    char input[TOTAL_MSG_LENGTH] = "";
    cin.getline(input, TOTAL_MSG_LENGTH);

    if(strcmp (input,"EXIT")) {
        /* todo error */
        return;
    }

    for (auto& client: clients)
    {
        string msgToSend = setMsgLength(SERVER_SHUT_DOWN_MSG) +
                SERVER_SHUT_DOWN_MSG;
        int socket = client.second->getSocketId();

        if (writeMsg(socket,msgToSend.c_str()) < 0)
        {
            /*todo error */
        }
//        if (send(socket, msgToSend.c_str(),
//                 strlen(msgToSend.c_str()),0) != strlen(msgToSend.c_str()))
//        {
//            /* todo error */
//        }
    }
    cout << "EXIT command is typed: server is shutting down\n";

    _exit(1);
}

void WhatsappServer::excCommand(string clientInput, ClientInfo * client) {
    vector<string> command;
    command = splitString(clientInput, " ");

    if (command.size() > 0)
    {
        if(!command[0].compare("who"))
        {
            if (command.size() != 1)
            {
                /* todo error */
            } else {

                who(client);
            }
        }
        else if (!command[0].compare("send"))
        {

            if (command.size() < 3)
            {
                /* todo error */

            } else
            {
                sendMsgBetweenClients(client,command[1],command);
            }
        }
        else if (!command[0].compare("create_group"))
        {
            if (command.size() < 3)
            {
                /* todo error */
            } else
            {
                createGroup(command[1],command[2],client);
            }


        }
        else if (!command[0].compare("exit"))
        {
            if (command.size() != 1)
            {
                /* todo error */
            } else {
                shutClientDown(client);
            }
        }
        else
        {
            /*todo error */
        }
    }
    else
    {
        /* todo error */
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

int WhatsappServer::writeMsg(int socketNum, const char *buffer) {
    int msgLength = strlen(buffer);
    int bcount = 0;       /* counts bytes read */
    int br = 0;           /* bytes read this pass */

    while (bcount < msgLength) { /* loop until full buffer */
        br = write(socketNum, buffer, msgLength-bcount);
        if (br >= 0){
            bcount += br;
            buffer += br;
        } else {
            return -1;
        }
    }

    return bcount;
}



/* ----------- Helper Functions ------------------ */


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

vector<string> WhatsappServer::splitString(string stringToSplit,
                                           string character) {
    string name = "me";
    vector<string> splitVector;
    unsigned long charLocation = stringToSplit.find(character);

    while ((charLocation != std::string::npos))
    {
        string name = stringToSplit.substr(0, charLocation);

        splitVector.push_back(name);
        stringToSplit = stringToSplit.substr(charLocation + 1);
        charLocation = stringToSplit.find(character);
    }

    splitVector.push_back(stringToSplit);
    return splitVector;
}

string WhatsappServer::setMsgLength(string msg) {
    string length = "";
    int size = msg.length();
    if (size < 10)
    {
        length = "00";

    } else if (size < 100)
    {
        length = "0";
    }
    length = length + to_string(size);
    return length;
}

void WhatsappServer::deleteClient(ClientInfo *client) {
    if (client->getSocketId() == maxId)
    {
        maxId = 0;
        for (auto& client: clients)
        {
            int socket = client.second->getSocketId();
            if (socket > maxId)
            {
                maxId = socket;
            }
        }
    }

    clients.erase (client->getName());
    FD_CLR(client->getSocketId(),&openedSockets);
    delete client;
}



/* ------------------ Commands ------------------ */


void WhatsappServer::createGroup(string groupName, string members,
                                 ClientInfo *client) {
    string msgToSend = "";

    if (isClientExist(groupName)== 0 || isGroupExist(groupName) == 0)
    {
        /* todo error */
        msgToSend += "ERROR: failed to create group \"" + groupName + "\".\n";
        return;
    } else
    {
        if (members != "")
        {
            return;
        }

        vector<string> clientsNames = splitString(members, ",");
        clientsNames.push_back(client->getName());
        Group * group = new Group(groupName);
        if (group->setGroupMembers(clientsNames, clients) < 0)
        {
            /*todo error */
        }
        groups.insert(make_pair(groupName,group));
        msgToSend += "Group \"" + groupName + "\"was created successfully.\n";

    }

    cout <<  client->getName() << ": " << msgToSend << flush;
    msgToSend = setMsgLength(msgToSend) + msgToSend;
    writeMsg(client->getSocketId(), msgToSend.c_str());
}


/**
 * Sends to the caller a list (might be empty) of currently connected
 * client names (alphabetically order), separated by comma without spaces.
 * @param senderName
 */
void WhatsappServer::who(ClientInfo * client){
    string msg("");
    bool first = true;

   cout << client->getName() << ": Requests the currently connected " <<
        "client names\n";
    /* todo sort ? */

    auto it = clients.begin();

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

    msg+= "\n";
    msg = setMsgLength(msg) + msg;

    if (writeMsg(client->getSocketId(), msg.c_str()) < 0)
    {
        /* todo error */
        return;
    }
//    if (send(client->getSocketId(), msg.c_str(), strlen(msg.c_str()),0) !=
//        strlen(msg.c_str()))
//    {
//        /* todo error */
//    }
}

void WhatsappServer::sendMsgBetweenClients(ClientInfo * client,
                                           string receiverName,
                                           vector<string> msg)
{
    string msgToSend= "";
    for (int i = 2; i < msg.size(); i++)
    {
        msgToSend += msg[i] + " ";
    }

    ClientInfo * receiver;
    bool foundReceiver = false;

    for (auto& client : clients)
    {
        if (client.first == receiverName)
        {
            receiver = client.second;
            foundReceiver = true;

        }
    }

//    cout << receiverName << flush;
//    cout << foundReceiver << flush;

    if (!foundReceiver)
    {
        msgToSend = setMsgLength(SENT_ERROR_MSG) + SENT_ERROR_MSG;
        if (writeMsg(client->getSocketId(),msgToSend.c_str()) < 0)
        {
            /* todo error*/
        }
        return;
    }


    string successMsg = client->getName() + ": \"" + msgToSend + "\" " +
            "was sent successfully to " + receiverName + "\n";

    msgToSend = client->getName() + ": " + msgToSend + "\n";
    msgToSend = setMsgLength(msgToSend) + msgToSend;
    if (writeMsg(receiver->getSocketId(),msgToSend.c_str()) < 0)
    {
        /* todo error*/
        return;
    } else
    {
        msgToSend = setMsgLength(SENT_SUCCESS_MSG) + SENT_SUCCESS_MSG;
        if (writeMsg(client->getSocketId(),msgToSend.c_str()) < 0)
        {
                /* todo error */
        }
    }

    cout << successMsg << flush;

}

void WhatsappServer::shutClientDown(ClientInfo *client) {
         string msg = setMsgLength(UNREGISTER_SUCCESS_MSG) + UNREGISTER_SUCCESS_MSG;
         if (writeMsg(client->getSocketId(), msg.c_str()) < 0)
         {
             /* todo error */
        return;
    }
//    if (send(client->getSocketId(), msg.c_str(), strlen(msg.c_str()),0) !=
//        strlen(msg.c_str())) {
//        /* todo error */
//    }

    deleteClient(client);

    cout << client->getName() << ": Unregistered successfully.\n";

}

