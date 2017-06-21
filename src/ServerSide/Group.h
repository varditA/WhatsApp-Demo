//
// Created by Adi Yehezkeli on 2017/06/18.
//

#ifndef SRC_GROUP_H
#define SRC_GROUP_H
#include "ClientInfo.h"
#include <set>
#include <string>

class ClientInfo;
using namespace std;

class Group
{
private:
    string name;
    set<ClientInfo*> clients;

public:
    Group(string groupName, set<ClientInfo*>);
    ~Group();



};


#endif //SRC_GROUP_H
