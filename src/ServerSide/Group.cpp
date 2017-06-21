//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include "Group.h"

Group::Group(string groupName, set<ClientInfo*> clientsInGroup) {
    this->name = groupName;
    for (ClientInfo * client: clientsInGroup)
    {
        client->addGroup(this);
        clients.insert(client);
    }
}
