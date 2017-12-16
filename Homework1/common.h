// Name for the queue of messages going to the server.
#define SERVER_QUEUE "/jmtimper-server-queue"
 
// Name for the queue of messages going to the current client.
#define CLIENT_QUEUE "/jmtimper-client-queue"
 
// Maximum number of words the server can store (capacity)
#define LIST_CAP 6
 
// Maximum length of a word on the server's list
#define WORD_MAX 20
 
// Maximum length for a message in the queue
// (Long enough for a report of all the words)
#define MESSAGE_LIMIT 1024

//#define _POSIX_SOURCE 
 
//status of the send and recieve states
int status = 0;
