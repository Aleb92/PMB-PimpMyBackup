#ifndef SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_
#define SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_

//FIXME meglio enum??? E la type safety?

namespace server {
#define MOVE		(1)
#define CREATE		(1<<1)
#define DELETE		(1<<2)
#define CHMOD		(1<<3)
#define TIMESTAMP	(1<<4)
#define VERSION		(1<<5)
#define MODIFIER	(1<<6)
#define WRITE		(1<<7)
}


#endif /* SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_ */
