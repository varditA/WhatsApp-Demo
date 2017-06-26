//
// Created by Vardit on 19/06/2017.
//

#ifndef EX5_CLIENTINFO_H
#define EX5_CLIENTINFO_H

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "Group.h"


class Group;
using namespace std;

/* represents a client that connected by the server */

class ClientInfo{

private:
    int socketId;               /* the client's socket num */
    const string name;          /* the client's name */
    vector <Group *> groups;    /* the client's groups */

public:
    ClientInfo(int socket, const string name);
    ~ClientInfo();

    inline int getSocketId() const {
        return socketId;
    }

    inline const string &getName() const {
        return name;
    }

    inline vector<Group*> getGroups() const {
        return groups;
    }

    inline void addGroup(Group * group) {
        this->groups.push_back(group);
    }

    void deleteFromAllGroups();



};

#endif //EX5_CLIENTINFO_H
