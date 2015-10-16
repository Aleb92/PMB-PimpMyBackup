
#ifndef SOURCE_UTILITIES_INCLUDE_LOG_HPP_
#define SOURCE_UTILITIES_INCLUDE_LOG_HPP_

#include <ostream>

std::ostream& operator<<(std::ostream& os, const FILE_NOTIFY_INFORMATION& obj)
{
  os << obj.Action << ":" << obj.FileName << ":" << std::endl;
  return os;
}

class log {

public:
	log(){ }
	void operator()(){ }
	~log(){ }
};

#endif /* SOURCE_UTILITIES_INCLUDE_LOG_HPP_ */
