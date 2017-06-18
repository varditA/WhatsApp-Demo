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
    int create_group();
private:
    vector<WhatsappClient *> clients;
    vector<Group *> groups;
};


#endif //SRC_SERVER_H
