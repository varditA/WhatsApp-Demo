//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include "Group.h"

Group::Group(string groupName) {
    this->name = groupName;
}

int Group::setGroupMembers(vector <string> clientsNames,
                            map<string, ClientInfo *> clientsInServer) {

    for (string name : clientsNames)
    {
        const auto & clientFound = clientsInServer.find(name);
        if (clientFound != clientsInServer.end())
        {
            this->clientsInGroup.insert(clientFound->second);
            clientFound->second->addGroup(this);
        } else
        {
            /* todo error  - the client isn't existed*/
            return -1;
        }
    }


    if (this->clientsInGroup.size() < 2)
    {
        return -1;
    }

    return 0;

}

set<ClientInfo*> Group::getGroupMember() {
    return clientsInGroup;
}

void Group::removeFromGroup(ClientInfo *client) {
    clientsInGroup.erase(client);
}

Group::~Group() {
    clientsInGroup.clear();
}