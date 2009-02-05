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

//! @brief Init the agrawala algorithm processor
//! @param port The port we want to bind the socket on
void agrawala_init(const int port);

//! @brief Runs the agrawala algorithm
void agrawala_run();

//! @brief Properly terminate the agrawala processing
void agrawala_close();

#endif

