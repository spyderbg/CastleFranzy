#ifndef __GL_XPLAYER_USER_OBSERVER_H__
#define __GL_XPLAYER_USER_OBSERVER_H__

namespace gllive
{
	class GLXPlayerUserObserver
	{

	public:

		virtual void OnUserNetworkError() = 0;

		virtual void OnUserFunctionSuccess( int func_id, char * data ) = 0;

		virtual void OnUserFunctionError( int func_id, int error_code ) = 0;
	};
}

#endif