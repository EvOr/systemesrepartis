/** 
 * @file agrawala.h
 * @brief The Ricart and Agrawala algorithm header
 * @author Julien Dessaux & Jan Villeminot
 * @date 2009-01-17
 */

#ifndef _AGRAWALA_H_
#define _AGRAWALA_H_
#define ACK 1
#define REQ 0 
typedef struct message{
   int type;
   int clock;
   int client;
}message_t;

void agrawala_init(const int port);

void agrawala_run();

void agrawala_close();

#endif

