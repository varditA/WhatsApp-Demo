Server - 
________

Has two helper classes: Group and ClientInfo 
They help it manage all the clients and the groups that connected to it. 
It owns two maps, their key is the group/client name so it can be found easily if someone send a message to another.

Client -
________

Responsible from receiving the input and check it - if it's invalid, return a message without contacting the server. 
If it's valid, send it to the server. Moreover, can receive information from the server and send it to the user.