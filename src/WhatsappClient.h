//
// Created by Adi Yehezkeli on 2017/06/18.
//

#ifndef SRC_CLIENT_H
#define SRC_CLIENT_H

#include <vector>
#include <string>
#include "src/WhatsappServer.h"

using namespace std;

class WhatsappClient
{
public:
    /**
     * The constructor of the class
     */
    WhatsappClient(char *clientName, char *serverAddress, char *serverPort);

    /**
     * The destructor of the class
     */
    ~WhatsappClient();

    /**
     * Wait for client input
     */
    void getCommand(string *buf, char *userInput);  /* todo what should be done here */

    inline int getSocketId(){ return socketId; }
    void setSocketId(int socketId);
    struct sockaddr_in getSa();

private:
    int socketId;
    struct sockaddr_in sa;

    char *clientName;
    char *serverAddress;
    char *serverPort;

    void clientInit();

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
     * @param clientNames a vector of strings of the clients names
     */
    void create_group(string groupName, string clientNames, string *buf);

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
    void send(string name, string msg, string *buf);

    /**
     * Sends a request (to the server) to receive a list (might be empty) of currently connected
     * client names (alphabetically order), separated by comma without spaces.
     */
    void who(string *buf);

    /**
     * Unregisters the client from the server and removes it from all groups. After the server
     * unregistered the client, the client should print “Unregistered successfully” and then exit(0).
     */
    void exit(string *buf);

    /**
     * Prints a message to the client
     * @param msg
     */
    void clientPrint(string msg);

    /**
     * Splits a string into a vector of strings by a character
     * @param stringToSplit the string to split
     * @param character the character to split by
     * @return a vector containing the split strings
     */
    vector<string> splitString(vector<string> splitVector, string stringToSplit, string character);
};

#endif //SRC_CLIENT_H
