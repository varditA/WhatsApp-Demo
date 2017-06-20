//
// Created by Adi Yehezkeli on 2017/06/18.
//

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
        server->activateServer(PORT_NUM);
        server->waitForConnection();
    }
    catch (exception e){
        /*todo error */
        return -1;
    }

    return 0;
}

/* ------------------------ Public Functions ------------------------------------------ */



WhatsappServer::WhatsappServer() {
    FD_ZERO(&openedSockets);
}

WhatsappServer::~WhatsappServer() {
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
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = hp->h_addrtype;
    memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_port= htons(portNum);

    if (createSocket() != 0)
    {
        return -1;
    }

    if (bind(socketId , (struct sockaddr *)&sa , sizeof(struct sockaddr_in)) < 0) {
//        close(socketId);

        /*todo error*/
        return(-1);
    }

    socketAdd = sizeof(sa);         /* todo:  what for? */
    listen(socketId, 10); /* max # of queued connects */
    return 0;
}


/**
 * The function waits for new client. For every new client, it creats
 * new socket and add it to the server's clients.
 * @return
 */
int WhatsappServer::waitForConnection() {
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
    socketId= socket(AF_INET, SOCK_STREAM, 0);
    if (socketId < 0) {
        /* todo error */
        return(-1);
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


void WhatsappServer::addNewClient()  {
    int newSocketNum = getConnection();
    if (newSocketNum != 0)
    {
        FD_SET(newSocketNum, &openedSockets);
        char buffer[TOTAL_MSG_LENGTH];
        if (readMsg(newSocketNum,buffer) < 0)
        {
            /* can't get the name*/
            return;
        }

        string name(buffer);
        name = name.substr(0, name.find(" "));

//        if(recv(newSocketNum, name , 30 , 0) < 0)
//        {
//            /* todo error */
//        }

        if (isClientExist(name)== 0 || isGroupExist(name) == 0)
        {
            if (send(newSocketNum, CONNECT_FAILURE_MSG, strlen(CONNECT_FAILURE_MSG), 0) != strlen(CONNECT_FAILURE_MSG))
            {
                /* todo error */
            }
        }

        ClientInfo * client = new ClientInfo(newSocketNum,name);
        clients.insert(make_pair(name,client));

        if (send(newSocketNum, CONNECT_SUCCESS_MSG, strlen(CONNECT_SUCCESS_MSG), 0) != strlen(CONNECT_SUCCESS_MSG))
        {
            /* todo error */
        } else
        {
            string message = string(name) + string(" connected."); /* todo fix it */
            cout << message;
        }
    }
}

void WhatsappServer::getMsgFromClient() {
//            for (pair<int,ClientInfo*> client: clients)
//            {
//                int clientSocket = client.second->getSocketId();
//                if (FD_ISSET(clientSocket, &openedSockets))
//                {
//                    char message[1000];
//                    if(recv(newSocketNum, message , 30 , 0) < 0)
//                    {
//                        /* todo error */
//                    }
//                    handleMsg(client, message);
//                }
//            }
}

int WhatsappServer::readMsg(int socketNum, char *buffer) {
//    char temp[TOTAL_MSG_LENGTH];
    char * temp = new char[TOTAL_MSG_LENGTH];
    char ** pointerBuffer = &temp;
    int bcount;       /* counts bytes read */
    int br;               /* bytes read this pass */
    bcount= 0; br= 0;
    int found = 1;

    while (found ==  string::npos || bcount < TOTAL_MSG_LENGTH)
    {
        br = read(socketNum, temp, 1000);
        if (br >= 0)
        {
            bcount += br;
            temp += br;
        }
        else
        {
            return -1;
        }
        string str(temp);
        found = str.find(" ");
    }

    string str(*pointerBuffer);
    string size(str.substr(0,found));
    int msg_length = atoi(str.c_str());
    bcount -= size.length() + 1;

    while (bcount < msg_length) { /* loop until full buffer */
        br = read(socketNum, temp, msg_length-bcount);
        if (br >= 0){
            bcount += br;
            temp += br;
        } else {
            return -1;
        }
    }

    if (memcpy(buffer + size.length(), temp, msg_length) < 0)
    {
        /* todo error */
    }
    delete temp;
    return(bcount);
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