//
// Created by Vardit on 19/06/2017.
//

#include "ClientInfo.h"

ClientInfo::ClientInfo(int socket, const string name): socketId(socket), name(name) {}

ClientInfo::~ClientInfo() {
    deleteFromAllGroups();
    close(socketId);
}

void ClientInfo::deleteFromAllGroups() {
    for (auto& group: groups)
    {
        group->removeFromGroup(this);
    }
}