#ifndef __GL_XPLAYER_USER_FRIEND_H__
#define __GL_XPLAYER_USER_FRIEND_H__

#include "../Common/GLXPlayerWebComponent.h"
namespace gllive
{
	/**
	* Class GLXPlayerUserFriend. 
	* This class is designed to represent a buddy.
	*
	* @author  World Online Team <<online-adm@gameloft.com>>
	* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
	* @version 1.0.0 
	*  
	*/
	class GLXPlayerUserFriend : public GLXPlayerWebComponent
	{
	public:
		/**
		* GLXPlayerUserFriend constructor.
		* Create an instance of GLXPlayerUserFriend.
		*/
		GLXPlayerUserFriend(void);
		
		/**
		* GLXPlayerUserFriend destructor.
		* Destroy an instance of GLXPlayerUserFriend.
		*/
		virtual ~GLXPlayerUserFriend(void);	

		/**
		* Function sendAddUserFriend.
		* Add a friend and wait for friend confirmation.
		* @param isUserNumber Is use usernumber or username
		* @param userName Name of user to add as a friend.
		* @param msg Option parameter. Invitation message.
		* @return Success: |r|s / Fail:  |r|e|error code
		*/
	#ifdef USE_USER_NUMBER
		bool sendAddUserFriend(char* userName, char* msg = NULL, bool isUserNumber = false);	
	#else
		bool sendAddUserFriend(char* userName, char* msg = NULL);	
	#endif	//USE_USER_NUMBER
		/**
		* Function sendConfirmUserFriend.
		* Allow or refuse to add current user as a friend.
		* @param userName Name of user to add as a friend.
		* @param confirmValue 1: Allow, 0: Refuse. 
		* @see sendAddUserFriend() 
		* @return Success: |r|s / Fail: |r|e|error code
		*/
		bool sendConfirmUserFriend(char* userName ,int confirmValue);	
		
		/**
		* Function sendDelUserFriend.
		* Delete one of my friend.
		* @param userName Name of user to delete.	
		* @return Success: |r|s / Fail: |r|e|error code
		*/
		bool sendDelUserFriend(char* userName);	
		
		/**
		* Function sendGetUserFriends.
		* Get user's friend list.
		* @param startIndex Start index to read..	
		* @param count Read count.	
		* @see processFriendsList()	
		* @return Success: |r|s|c|total friends count|l|friend list / Fail: |r|e|error code
		*/
		bool sendGetUserFriends(int startIndex = 0, int count = 0);		
		
		/**
		* Function sendGetUserFriendsCount.
		* Get user's friends count.	
		* @return Success: |r|s|friends count / Fail: |r|e|error code
		*/
		bool sendGetUserFriendsCount();	

		/**
		* Function GetFriendsCount.
		* Return number of friend in current friends list.	
		* @see sendGetUserFriendsCount()
		* @see processFriendsList()
		* @return m_friendsCount
		*/
		int GetFriendsCount();
		
		/**
		* Function GetUserNumber.
		* Return name of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend name at the position indicated.
		*/
		char * GetUserNumber(int position);
		
		/**
		* Function GetName.
		* Return name of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend name at the position indicated.
		*/
		char * GetName(int position);
		
		/**
		* Function GetCountry.
		* Return country of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend country at the position indicated.
		*/
		char * GetCountry(int position);	
		
		/**
		* Function GetReputation.
		* Return reputation of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend reputation at the position indicated.
		*/
		double GetReputation(int position);
		
		/**
		* Function GetBadReputationCount.
		* Return bad reputation count of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend reputation at the position indicated.
		*/
		int GetBadReputationCount(int position);
		
		/**
		* Function GetGoodReputationCount.
		* Return good reputation count of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend reputation at the position indicated.
		*/
		int GetGoodReputationCount(int position);
		
		/**
		* Function GetState.
		* Return state of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend state at the position indicated.
		*/
		int GetState(int position);	
		
		/**
		* Function GetCurrentGame.
		* Return current game of friend in current friends list, indicated by position.	
		* @param position Position of friend in the list.	
		* @see sendGetUserFriends()	
		* @see processFriendsList()	
		* @return Friend current game at the position indicated.
		*/
		char * GetCurrentGame(int position);
		
		/**
		* Function GetTrophy.
		*  Return trophy of friend in current friends list, indicated by position.
		* @param position Position of friend in the list.
		* @see sendGetUserFriends()
		* @see processFriendsList()
		* @return Friend trophy game at the position indicated.
		*/
		int GetTrophy(int position);

		/**
		* Function GetTotalTrophyScore.
		*  Return total trophy score of friend in current friends list, indicated by position.
		* @param position Position of friend in the list.
		* @see sendGetUserFriends()
		* @see processFriendsList()
		* @return Friend trophy game at the position indicated.
		*/
		int GetTotalTrophyScore(int position);

		char * GetCurrentLanguage(int position);

		char * GetLastUpdateAvatar(int position);

		char * GetAvatarId(int position);

	protected:
		/**
		* Private function OnUpdateSuccess.
		* Handle success message returned from the Web server.	
		* @param func_id Function ID.
		* @return True / False.
		*/
		bool OnUpdateSuccess( int func_id );

		/**
		* Private function processFriendsList.
		* Process friend list if success message returned from the server.
		* @see sendGetUserFriends()
		* @param data Data returned from the server.
		*/
		void processFriendsList(char* data);

	private:
		/**
		* Private function clearFriendsList.
		* Clear user's friend list.
		*/
		void clearFriendsList();
		
		/**
		* Variable m_friendsCount.
		* Store the number of friend in the list. 
		*/
		int m_friendsCount;
		
		/**
		* Variable m_usernumber.
		* Store the usernumber list.
		*/
		char ** m_usernumber;
		
		/**
		* Variable m_name.
		* Store the name list.
		*/
		char ** m_name;
		
		/**
		* Variable m_name.
		* Store the country list.
		*/
		char ** m_nationality;
		
		/**
		* Variable m_reputation.
		* Store the reputation list .
		*/
		double * m_reputation;
		
		/**
		* Variable m_badEvaluationCount.
		* Store the reputation list .
		*/
		int * m_badEvaluationCount;
		
		/**
		* Variable m_goodEvaluationCount.
		* Store the reputation list .
		*/
		int * m_goodEvaluationCount;
		
		/**
		* Variable m_state.
		* Store the state list .
		*/
		int * m_state;
		
		/**
		* Variable m_current_game.
		* Store the current game list .
		*/
		char ** m_current_game;
		
		/**
		* Variable m_trophy.
		* Store the trophy list .
		*/
		int * m_trophy;
		
		/**
		* Variable m_totalTrophyScore.
		* Store total trophy score user list .
		*/
		int * m_totalTrophyScore;

		/**
		* Variable m_current_language.
		* Store the current friend language list .
		*/
		char ** m_current_language;

		char ** m_lastUpdateAvatar;

		/**
		* Variable m_avatarId.
		* Store the avatar id list.
		*/
		char ** m_avatarId;
	};
}
#endif
