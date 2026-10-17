#ifndef __GL_XPLAYER_COMPONENT_H__
#define __GL_XPLAYER_COMPONENT_H__

#include "../Porting.h"
#include "GLXPlayerObserver.h"

/**
 * Class GLXPlayerComponent
 * It is the basic class of all the XPlayer components,
 * such as GLXPlayerUser, GLXPlayerChat, GLXPlayerMultiplayer, and so on.
 * If you want to add a new XPlayer function, please inherit from this class.
 * One component should only focus on one field.
 * Every XPlayer interface ( e.g. GameloftLive ) provided by Online Team
 * should be made up of one or several XPlayer components. 
 *
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author Lu Tie Hua <<tiehua.lu@gameloft.com>>
 * @author Tran Thai Phi <<phi.tranthai@gameloft.com>> 
 * @version 1.0.0 
 */
namespace gllive {
	
	class GLXPlayerComponent
		{
			
		public:
			
			/**
			 * GLXPlayerComponent constructor. 
			 */
			GLXPlayerComponent(void);
			
			/**
			 * GLXPlayerComponent destructor. 
			 */
			virtual ~GLXPlayerComponent(void);
			
			/**
			 * Function Update.
			 * Update connection, send and receive data.
			 * Should be implemented in subclass.	
			 */
			virtual void Update() = 0;
			
			/**
			 * Function RegisterObserver.
			 * The subclass can implement new observer inheritted from GLXPlayerObserver.
			 * @param observer An instance of GLXPlayerObserver.
			 */
			void RegisterObserver( GLXPlayerObserver * observer );
			
			/**
			 * Function SetGGI.
			 * Set the game's GGI, which used for send request to server.
			 * @param ggi The game's GGI.
			 */
			void SetGGI( unsigned long ggi );
			
			/**
			 * Function GetGGI.
			 * Get the GGI of this game.
			 * @return The game's GGI.
			 */
			unsigned long GetGGI();
			
			/**
			 * Function SetUID.
			 * Set the User ID of this game.
			 * @param uid The User ID.
			 */
			void SetUID( char * uid );
			
			/**
			 * Function GetUID.
			 * Get current user UID.
			 * @return The User ID.
			 */
			char * GetUID();
			
		protected:
			
			/**
			 * Variable m_observer.
			 * The observer of GLXPlayerComponent instance.
			 * @see RegisterObserver()
			 */
			GLXPlayerObserver * m_observer;
			
			/**
			 * Variable m_ggi.
			 * Gameloft Game Identifier
			 * @see SetGGI()
			 * @see GetGGI()
			 */
			unsigned long m_ggi;
			
			/**
			 * Variable m_uid.
			 * User Identifier
			 * @see SetUID()
			 * @see GetUID()
			 */
			char * m_uid;
		};
}

#endif
