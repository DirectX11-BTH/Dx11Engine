#include "GBufferHandler.h"

void GBufferHandler::init(int width, int height)
{
	for (int i = 0; i < 4; i++ )
	{
		buffers[i].init(width, height);
	}
}
