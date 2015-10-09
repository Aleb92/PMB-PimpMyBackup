/*
 * taskqueue.h
 *
 *  Created on: 09 ott 2015
 *      Author: Marco
 */

#ifndef SOURCE_CLIENT_TASKQUEUE_H_
#define SOURCE_CLIENT_TASKQUEUE_H_

namespace client {

	class task_queue {
	public:
		task_queue(const char*);
		~task_queue();
		void push();
		//pop();

	};

} /* namespace client */

#endif /* SOURCE_CLIENT_TASKQUEUE_H_ */
