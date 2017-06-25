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

class Group
{
private:
    string name;
    set<ClientInfo*> clients;




public:
    Group(string groupName);
    ~Group();

    set<ClientInfo*> getGroupMember();

    int setGroupMembers(vector <string> clientsNames,
                               map<string, ClientInfo *> clients);
    void removeFromGroup(ClientInfo * client);



};


#endif //SRC_GROUP_H
