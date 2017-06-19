//
// Created by Adi Yehezkeli on 2017/06/18.
//

#ifndef SRC_SERVER_H
#define SRC_SERVER_H


#include "WhatsappClient.h"
#include "Group.h"

class WhatsappServer
{
public:
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
    vector<WhatsappClient *> clients;
    vector<Group *> groups;
};


#endif //SRC_SERVER_H
