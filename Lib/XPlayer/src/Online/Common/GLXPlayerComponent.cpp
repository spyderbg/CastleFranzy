#include "../../../include/Common/GLXPlayerComponent.h"
//#include "memory.h"


/**
* GLXPlayerComponent constructor. 
*/
GLXPlayerComponent::GLXPlayerComponent()
{
	m_observer = NULL;
	m_ggi = 0;
	m_uid = NULL;
}

/**
* GLXPlayerComponent destructor. 
*/
GLXPlayerComponent::~GLXPlayerComponent(void)
{
	m_observer = NULL;
	m_ggi = 0;
	XP_API_DEL( m_uid );
}

/**
 * Function RegisterObserver.
 * The subclass can implement new observer inheritted from GLXPlayerObserver.
 * @param observer An instance of GLXPlayerObserver.
 */
void GLXPlayerComponent::RegisterObserver( GLXPlayerObserver * observer )
{
	m_observer = observer;
}

/**
 * Function SetGGI.
 * Set the game's GGI, which used for send request to server.
 * @param ggi The game's GGI.
 */
void GLXPlayerComponent::SetGGI( unsigned long ggi )
{
	m_ggi = ggi;
}

/**
 * Function GetGGI.
 * Get the GGI of this game.
 * @return The game's GGI.
 */
unsigned long GLXPlayerComponent::GetGGI()
{
	return m_ggi;
}

/**
 * Function SetUID.
 * Set the User ID of this game.
 * @param uid The User ID.
 */
void GLXPlayerComponent::SetUID( char * uid )
{
	XP_API_DEL( m_uid );
	m_uid = XP_API_STRNEW( uid );	
}

/**
 * Function GetUID.
 * Get current user UID.
 * @return The User ID.
 */
char * GLXPlayerComponent::GetUID()
{
	return m_uid;
}