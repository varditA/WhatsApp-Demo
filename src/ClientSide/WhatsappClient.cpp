//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include <iostream>
#include <zconf.h>
#include "WhatsappClient.h"
#include <sys/types.h>
#include <sys/socket.h>

string INVALIT_INPUT_MSG = "ERROR: Invalid input.\n";
string UNREGISTER_SUCCESS = "Unregistered successfully\n";
string CONNECTION_SUCCESS = "Connected Successfully.\n";

/**
 * The constructor of the class
 */
WhatsappClient::WhatsappClient(char *clientName, char *serverAddress, char *serverPort)
{
    string name(clientName);
    this->clientName = name;
    this->serverAddress = serverAddress;
    this->serverPort = serverPort;
}

/**
 * The destructor of the class
 */
WhatsappClient::~WhatsappClient(){
    delete myName;
}

int WhatsappClient::clientInit()
{
    int portNum = atoi(serverPort);
    if (setHostName() == -1)
    {
        /*todo error*/
        return -1;
    }

    hp = gethostbyname(myName);
    if (hp == NULL)
    {
        /* todo error */
        return -1;
    }
    memset(&sa, 0, sizeof(sa));
    memcpy((char *)&sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons((u_short)portNum);

    setSocketId(socket(AF_INET, SOCK_STREAM, 0));
    if (getSocketId() < 0) {
        /* todo error */
        return(-1);
    }

    if (connect(getSocketId(), (struct sockaddr *)&sa , sizeof(struct sockaddr_in)) < 0)
    {
        cout << "error connecting" << flush;
//        close(socketId);
        /*todo error*/
        return -1;
    }
    else {
        int length = clientName.length();
        string nameMsg("");
        nameMsg += to_string(length) + " " + clientName;

        /* add the length to the msg's beginning */
        if (send(socketId, nameMsg.c_str(), strlen(nameMsg.c_str()),0) !=
                strlen(nameMsg.c_str()))
        {
            /* todo error */
        } else
        {
            char buffer[MAX_HOSTNAME_LENGTH];
            int nbytes = TEMP_FAILURE_RETRY(recv(socketId, buffer,
                                                 MAX_HOSTNAME_LENGTH,0));
            if (nbytes > 0)
            {
                cout << buffer;

            } else
            {
                /* todo error */
            }
        }
    }

    return 0;
}

/**
 * Wait for client input
 */
void WhatsappClient::excCommand(char *userInput)
{
    vector<string> parameters;
    parameters = splitString(parameters, userInput, " ");
    string str = "";

    if (parameters.at(0) == "create_group") {
        if (parameters.size() != 3)
        {
            clientPrint(INVALIT_INPUT_MSG);
        } else {
            create_group(parameters.at(1), parameters.at(2));
        }
    } else if(parameters.at(0) == "send") {
        if (parameters.size() != 3)
        {
            clientPrint(INVALIT_INPUT_MSG);
        } else {
            sendMsg(parameters.at(1), parameters.at(2));
        }
    } else if(parameters.at(0) == "who") {
        if (parameters.size() != 1)
        {
            clientPrint(INVALIT_INPUT_MSG);
        } else {
            who();
        }
    } else if(parameters.at(0) == "exit") {
        if (parameters.size() != 1)
        {
            clientPrint(INVALIT_INPUT_MSG);
        } else {
            exit();
        }
    } else {
        clientPrint(INVALIT_INPUT_MSG);
    }
}

/**
 * Sends request to create a new group named “group_name” with <list_of_client_names> as group
 * members. “group_name” is unique (i.e. no other group or client with this name is allowed) and
 * includes only letters and digits.
 * <list_of_client_names> is separated by comma without any spaces.
 * For example:
 *      create_group osStuff david,tal,netanel,eshed
 * Notes:
 *      * It’s invalid to create a group without members.
 *        The client who sends the request should be part of the group (even if it doesn’t part
 *        of the received list).
 *      * A group must contain at least two members (including the client who creates the
 *        group).
 *      * If client name appears more than one time in the received list, consider it as
 *        one instance. For example: calling “create_group tal tal nati” is the same as
 *       “create_group tal nati”.
 * @param groupName a string of the group name
 * @param clientNames a string of of the clients names as arrived from the user
 */
void WhatsappClient::create_group(string groupName, string clientNames)
{
    vector<string> participants;
    splitString(participants, clientNames, ",");

    string toSend = "create_group " + groupName + " " + clientNames;
    toSend = toSend.length() + " " + toSend;
    if(send(socketId, toSend.c_str(), strlen(toSend.c_str()), 0) < 0)
    {
        //todo err
    }
    char input[1000];
    if (recv(socketId, input, 1000, 0) < 0)
    {
        //todo err
    } else { cout << input; }
}

/**
 * If name is a client name it sends <sender_client_name>: <message> only to the specified
 * client.
 * If name is a group name it sends <sender_client_name>: <message> to all group members (except
 * the sender client).
 * Notes:
 *      * Only a group member can send message to the group.
 *      * The received name must be different than the sender name (i.e. it’s invalid to send
 *        message only to yourself).
 * @param name the name of the group member to send to
 * @param msg the message to send
 */
void WhatsappClient::sendMsg(string name, string msg)
{
    string toSend = "send " + name + " " + msg;
//    *buf = temp.length() + " " + temp;
    toSend = toSend.length() + " " + toSend;
    if(send(socketId, toSend.c_str(), strlen(toSend.c_str()), 0) < 0)
    {
        //todo err
    }
    char input[1000];
    if (recv(socketId, input, 1000, 0) < 0)
    {
        //todo err
    } else { cout << input; }

}

/**
 * Sends a request (to the server) to receive a list (might be empty) of currently connected
 * client names (alphabetically order), separated by comma without spaces.
 */
void WhatsappClient::who()
{
    string toSend = "who";
//    *buf = temp.length() + " " + temp;
    toSend = toSend.length() + " " + toSend;
    if(send(socketId, toSend.c_str(), strlen(toSend.c_str()), 0) < 0)
    {
        //todo err
    }
    char input[1000];
    if (recv(socketId, input, 1000, 0) < 0)
    {
        //todo err
    } else { cout << input; }
}

/**
 * Unregisters the client from the server and removes it from all groups. After the server
 * unregistered the client, the client should print “Unregistered successfully” and then exit(0).
 */
void WhatsappClient::exit()
{
    string toSend = "exit";
//    *buf = temp.length() + " " + temp;
    toSend = toSend.length() + " " + toSend;
    if(send(socketId, toSend.c_str(), strlen(toSend.c_str()), 0) < 0)
    {
        //todo err
    }
    char input[1000];
    if (recv(socketId, input, 1000, 0) < 0)
    {
        //todo err
    } else { cout << input; }
}

/**
* Prints a message to the client
* @param msg
*/
void WhatsappClient::clientPrint(string msg)
{
    cout << msg;
}

/**
* Splits a string into a vector of strings by a character
* @param stringToSplit the string to split
* @param character the character to split by
* @return a vector containing the split strings
*/
vector<string> WhatsappClient::splitString(vector<string> splitVector, string stringToSplit,
                                           string character)
{
    unsigned long charLocation = stringToSplit.find(character);
    if (charLocation == string::npos)
    {
        splitVector.push_back(stringToSplit);
    }
    while (charLocation != string::npos)
    {
        string name = stringToSplit.substr(0, charLocation);
        splitVector.push_back(name);
        stringToSplit = stringToSplit.substr(charLocation);
        charLocation = stringToSplit.find(character);
    }

    return splitVector;
}

void WhatsappClient::setSocketId(int socketId)
{
    this->socketId = socketId;
}

struct sockaddr_in WhatsappClient::getSa()
{
    return sa;
}

/**
 * setting the host name
 * @return 0 if succeed and -1 if not.
 */
int WhatsappClient::setHostName() {
    myName = new char(MAX_HOSTNAME_LENGTH);
    if (gethostname(myName, MAX_HOSTNAME_LENGTH) != 0)
    {
        /* todo error */
        return -1;
    }
    return 0;
}


int main(int arg, char *argv[]) {

    WhatsappClient client(argv[1], argv[2], argv[3]);

    if (client.clientInit() < 0)
    {
        return 1;
    }


    char *userInput;
    while (true)
    {
        cin >> userInput;
        client.excCommand(userInput);
    }

    return 0;
}

