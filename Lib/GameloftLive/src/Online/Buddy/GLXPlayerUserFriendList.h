#ifndef __GL_XPLAYER_USER_FRIEND_LIST_H__
#define __GL_XPLAYER_USER_FRIEND_LIST_H__

#include "GLXPlayerUserFriend.h"
#include <string>
#include <vector>
#include <map>

/**
 * This class is designed to store all the friends.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerUserFriendList
{

public:

	GLXPlayerUserFriendList(void);

	~GLXPlayerUserFriendList(void);

	/**
	 * 
	 *
	 */
	GLXPlayerUserFriend * Find( char * name );

	bool Add( char * name, GLXPlayerUserFriend * user_friend );

	int Count();

	GLXPlayerUserFriend * Get( int index );

	void Clear();

private:

	typedef std::map< std::string, GLXPlayerUserFriend * > MAP_FRIEND;
	typedef std::map< std::string, GLXPlayerUserFriend * >::iterator IT_MAP_FRIEND;

	MAP_FRIEND m_list;

};

#endif