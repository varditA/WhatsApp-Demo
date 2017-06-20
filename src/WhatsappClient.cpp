//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include <iostream>
#include "WhatsappClient.h"

string INVALIT_INPUT_MSG = "ERROR: Invalid input.\n";
string UNREGISTER_SUCCESS = "Unregistered successfully";

/**
 * The constructor of the class
 */
WhatsappClient::WhatsappClient(string name, WhatsappServer *server)
{
    this->server = server;
    this->name = name;
}

/**
 * The destructor of the class
 */
WhatsappClient::~WhatsappClient(){}

/**
 * Wait for client input
 */
void WhatsappClient::listen()
{
    vector<string> parameters;
    string userInput = "";
    //TODO: wake up when there is a user input
    //TODO: save the user input in userInput
    parameters = splitString(parameters, userInput, " ");

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
            send(parameters.at(1), parameters.at(2));
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
    server->create_group(groupName, this->name, participants);
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
void WhatsappClient::send(string name, string msg)
{
    server->sendMsg(this->name, name, msg);
}

/**
 * Sends a request (to the server) to receive a list (might be empty) of currently connected
 * client names (alphabetically order), separated by comma without spaces.
 */
void WhatsappClient::who()
{
    server->who(this->name);
}

/**
 * Unregisters the client from the server and removes it from all groups. After the server
 * unregistered the client, the client should print “Unregistered successfully” and then exit(0).
 */
void WhatsappClient::exit()
{
    server->exit(this->name);
    //todo receive a signal indicating that the exit was preformed successfully, than exit
    clientPrint(UNREGISTER_SUCCESS);
    exit(0);
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