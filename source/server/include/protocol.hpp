#ifndef SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_
#define SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_

//FIXME meglio enum??? E la type safety?

namespace server {
#define OP_MOVE			(1)
#define OP_CREATE		(1<<1)
#define OP_DELETE		(1<<2)
#define OP_CHMOD		(1<<3)
#define OP_TIMESTAMP	(1<<4)
#define OP_VERSION		(1<<5)
#define OP_MODIFIER		(1<<6)
#define OP_WRITE		(1<<7)


#define OP_MOVE_BIT			(1)
#define OP_CREATE_BIT		(1<<1)
#define OP_DELETE_BIT		(1<<2)
#define OP_CHMOD_BIT		(1<<3)
#define OP_TIMESTAMP_BIT	(1<<4)
#define OP_VERSION_BIT		(1<<5)
#define OP_MODIFIER_BIT	(1<<6)
#define OP_WRITE_BIT		(1<<7)
}


#endif /* SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_ */
