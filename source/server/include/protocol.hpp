#ifndef SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_
#define SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_

#include <stdint.h>

namespace server {

	enum opcode : uint8_t {
		SYNC 		= 0,
		INVALID		= 0,
		CREATE		= 1<<0,
		MOVE		= 1<<1,
		REMOVE		= 1<<2,
		CHMOD		= 1<<3,
		APPLY		= 1<<4,
		VERSION		= 1<<5,
		LIST		= 1<<6,
		WRITE		= 1<<7
	};

	inline opcode operator|(opcode lh, opcode rh){ return static_cast<opcode>((uint8_t)lh | rh); }
	inline opcode operator&(opcode lh, opcode rh){ return static_cast<opcode>((uint8_t)lh & rh); }
	inline opcode operator^(opcode lh, opcode rh){ return static_cast<opcode>((uint8_t)lh ^ rh); }
	inline opcode operator~(opcode lh){ return static_cast<opcode>(~(uint8_t)lh); }
}


#endif /* SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_ */
