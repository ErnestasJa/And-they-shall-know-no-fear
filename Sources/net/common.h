#ifndef NET_COMMON_H
#define NET_COMMON_H

#include "message.h"

enum EClientFlags
{
	ECF_LOGGED_IN = BIT(0), ///was authenticated by the server
	ECF_IN_GAME = BIT(1), ///loaded level and everything necessary to begin
};

#endif // NET_COMMON_H
