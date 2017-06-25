//
// Created by Adi Yehezkeli on 2017/06/18.
//

#ifndef SRC_SERVER_H
#define SRC_SERVER_H

#include <iostream>
#include <map>
#include <set>
#include "Group.h"
#include "ClientInfo.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

const char CONNECT_SUCCESS_MSG []= "Connected successfully.\n";
const char CONNECT_FAILURE_MSG []= "Failure in connection.\n";
const char UNREGISTER_SUCCESS_MSG [] = "Unregistered successfully\n";
const char SERVER_SHUT_DOWN_MSG [] = "The server is shutting down\n";
const char SENT_SUCCESS_MSG [] = "Sent successfully.\n";
const char SENT_ERROR_MSG [] = "Error: failed to send.\n";


class WhatsappServer
{
public:
    WhatsappServer();
    ~WhatsappServer();

    /** open the connection to the server
 * @return -1 if the connection failed.
 *  if it has succeed, returns 0
 */
    int activateServer(int PortNum);

    /**
     * The function waits for new client. For every new client, it creats
     * new socket and add it to the server's clients.
     */
    int waitForConnection();


private:
    static const int MAX_HOSTNAME_LENGTH = 255;
    static const int TOTAL_MSG_LENGTH = 1000;
    char * myName;   /* the host's name */
    struct hostent *hp;                     /* the host's info */
    int maxId;                              /* the socket's max socket */

    struct sockaddr_in sa;                  /* the socket's info */
    int socketId;                           /* the socket's id */
    int socketAdd;                          /* the socket's address */

    fd_set openedSockets;         /* vector that contains the opened sockets */
    map<string, ClientInfo *> clients;
    map<string, Group *> groups;

    /* ------------- Setting The Server -------------- */

    /**
     * setting the host name
     * @return 0 if succeed and -1 if not.
     */
    int setHostName();

    /**
     * setting a structure of type hostent for the given host name
     * @return 0 if succeed and -1 if not
     */
    int setHostent();

    /**
     * creating a socket for the server's socket.
     * @return 0 if succeed and -1 if not
     */
    int createSocket();

    /**
     * open a new socket for a new client
     * @return the socket's num if succeed and 0 otherwise
     */
    int getConnection();

    /* ----------- Operating The Server -------------- */

    /**
     * check if the name is available. Sending a message to the client
     * accordingly and update the details.
     */
    void addNewClient();

    /**
     * find whom client has sent the message and handle it
     */
    void getMsgFromClient();

    /**
     * check if the recived command is "EXIT".
     * if it is, sending the clients a message that it's going to be shut down
     * and exit(1) when they replied with "ok"
     */
    void shutDownServer();

    /**
     * check the command and executing it.
     * @param clientInput - the client's command
     * @param client  - the client whom sent the command
     */
    void excCommand(string clientInput, ClientInfo * client);

    /**
     * reading a msg from a socket
     * @param socketNum - the socket that contains the msg
     * @param buffer - the buffer that we want to write to.
     * @return
     */
    int readMsg(int socketNum, char * buffer);

    /**
     * sending a message between clients.
     * @param senderName
     * @param receiverName
     * @param msg
     */

    /**
   * reading a msg from a socket
   * @param socketNum - the socket that contains the msg
   * @param buffer - the buffer that we want to write to.
   * @return
   */
    int writeMsg(int socketNum, const char * buffer);


    /* ----------- Helper Functions ------------------ */


    /**
    * @return 0 if client is existed and -1 if not
    */
    int isClientExist(string);

    /**
    * @return 0 if group is existed and -1 if not
    */
    int isGroupExist(string);

    string setMsgLength(string msg);

    vector<string> splitString(string stringToSplit, string character);

    void deleteClient(ClientInfo * client);

    /* ------------------ Commands ------------------ */

    /**
     * creating the groups by the group name and its members.
     * @param groupName
     * @param members
     * @param client
     */
    void createGroup(string groupName, string members, ClientInfo * client);

    /**
     * Sends to the caller a list (might be empty) of currently connected
     * client names (alphabetically order), separated by comma without spaces.
     * @param  the caller's name
     */
    void who(ClientInfo * client);

    /**
 * sending a message between clients.
 * @param senderName
 * @param receiverName
 * @param msg
 */
    void sendMsgBetweenClients(ClientInfo * client, string receiverName,
                               vector<string> msg);

    int sendMsgToClient(ClientInfo * receiver, string message);

    /**
     * closing the user's socket and release all its sources.
     * @param client - the client who needs to be exited.
     */
    void shutClientDown(ClientInfo * client);


};


#endif //SRC_SERVER_H
