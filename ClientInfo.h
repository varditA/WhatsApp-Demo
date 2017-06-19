//
// Created by Vardit on 19/06/2017.
//

#ifndef EX5_CLIENTINFO_H
#define EX5_CLIENTINFO_H

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "Group.h"

using namespace std;

class ClientInfo{

private:
    int socketId;
    const string name;
    vector <Group *> groups;

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


};

#endif //EX5_CLIENTINFO_H
