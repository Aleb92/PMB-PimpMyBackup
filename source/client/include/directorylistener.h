

#ifndef SOURCE_CLIENT_DIRECTORYLISTENER_H_
#define SOURCE_CLIENT_DIRECTORYLISTENER_H_

namespace client {

	class directory_listener {

		HANDLE dir;

	public:
		directory_listener(const char*);
		void scan(void);
		~directory_listener();
	};

}

#endif /* SOURCE_CLIENT_DIRECTORYLISTENER_H_ */
