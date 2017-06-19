//
// Created by Adi Yehezkeli on 2017/06/18.
//

#include "WhatsappServer.h"

void WhatsappServer::create_group(string groupName, string callerName, vector<string> members){}

void WhatsappServer::sendMsg(string senderName, string receiverName, string msg){}

/**
 * Sends to the caller a list (might be empty) of currently connected
 * client names (alphabetically order), separated by comma without spaces.
 * @param senderName
 */
void WhatsappServer::who(string caller){}

void WhatsappServer::exit(string caller){}