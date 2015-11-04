#ifndef SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_
#define SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_

namespace server {

	enum opcode : unsigned char {
		LIST		= 0,
		MOVE		= 1<<0,
		CREATE		= 1<<1,
		REMOVE		= 1<<2,
		CHMOD		= 1<<3,
		TIMESTAMP	= 1<<4,
		VERSION		= 1<<5,
		MODIFIER	= 1<<6,
		WRITE		= 1<<7
	};

	inline opcode operator|(opcode lh, opcode rh){ return static_cast<opcode>(lh | rh); }
	inline opcode operator&(opcode lh, opcode rh){ return static_cast<opcode>(lh & rh); }
	inline opcode operator~(opcode lh){ return static_cast<opcode>(~lh); }

}


#endif /* SOURCE_SERVER_INCLUDE_PROTOCOL_HPP_ */
