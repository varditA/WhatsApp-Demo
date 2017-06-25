//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include "Group.h"

Group::Group(string groupName) {
    this->name = groupName;
}

int Group::setGroupMembers(vector <string> clientsNames,
                            map<string, ClientInfo *> clients) {
    /* todo implement */
    for (string name : clientsNames)
    {
        const auto & clientFound = clients.find(name);
        if (clientFound != clients.end())
        {
            this->clients.insert(clientFound->second);
            clientFound->second->addGroup(this);
        } else
        {
            /* todo error  - the client isn't existed*/
            return -1;
        }
    }

    return 0;

}

void Group::removeFromGroup(ClientInfo *client) {
    clients.erase(client);
}

Group::~Group() {
    /*todo remove all clients*/
}