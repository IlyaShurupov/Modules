
#pragma once

#include "queue.h"

struct AndroidApi {
	
	union Event {
		
		enum Type : tp::uint1 {
			EV_NONE = 0,
		} type = EV_NONE;

		struct None {

		} none;


	};

	tp::Queue<Event> events;

};

extern AndroidApi* gAndroidApi;