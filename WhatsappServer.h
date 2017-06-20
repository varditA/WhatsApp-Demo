//
// Created by Adi Yehezkeli on 2017/06/18.
//

#ifndef SRC_SERVER_H
#define SRC_SERVER_H

#include <sys/types.h>
#include <map>
#include <afxres.h>                                 /*todo ???  check for bug*/
#include "Group.h"
#include "ClientInfo.h"

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

    void create_group(string groupName, string callerName, vector<string> members);
    void sendMsg(string senderName, string receiverName, string msg);

    /**
     * Sends to the caller a list (might be empty) of currently connected
     * client names (alphabetically order), separated by comma without spaces.
     * @param senderName
     */
    void who(string caller);

    void exit(string caller);
private:
    static const int MAX_HOSTNAME_LENGTH = 255;
    char myName[MAX_HOSTNAME_LENGTH + 1];   /* the host's name */
    struct hostent *hp;                     /* the host's info */

    struct sockaddr_in sa;                  /* the socket's info */
    int socketId;                           /* the socket's id */
    int socketAdd;                          /* the socket's address */

    fd_set openedSockets;                   /* vector that contains the opened sockets */
    map<int, ClientInfo *> clients;
    vector<Group *> groups;


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
     * creating a socket.
     * @return 0 if succeed and -1 if not
     */
    int createSocket();

    /**
     * The function waits for new client. For every new client, it creats
     * new socket and add it to the server's clients.
     * @return
     */
    int waitForClients();

    /**
     * open a new socket for a new client
     * @return the socket's num if succeed and 0 otherwise
     */
    int getConnection();

};


#endif //SRC_SERVER_H
