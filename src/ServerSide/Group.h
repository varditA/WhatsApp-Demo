//
// Created by Adi Yehezkeli on 2017/06/18.
//

#ifndef SRC_GROUP_H
#define SRC_GROUP_H
#include "ClientInfo.h"
#include <set>
#include <map>
#include <string>

class ClientInfo;
using namespace std;

/* represents a group of clients that is saved by the server */

class Group
{
private:
    string name;
    set<ClientInfo*> clientsInGroup;

public:
    Group(string groupName);
    ~Group();

    set<ClientInfo*> getGroupMember();

    int setGroupMembers(vector <string> clientsNames,
                               map<string, ClientInfo *> clientsInServer);
    void removeFromGroup(ClientInfo * client);



};


#endif //SRC_GROUP_H
