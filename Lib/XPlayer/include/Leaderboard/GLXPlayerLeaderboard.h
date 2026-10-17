#ifndef __GL_XPLAYER_LEADERBOARD_H__
#define __GL_XPLAYER_LEADERBOARD_H__

#include "../Porting.h"
#include "../Common/GLXPlayerWebComponent.h"
#include "memory.h"

/**
* Class GLXPlayerLeaderboard. 
* Class GLXPlayerLeaderboard is designed for uploading your game score and get the leader board of the game.
*
* @author  World Online Team <<online-adm@gameloft.com>>
* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
 * @version 1.0.0 
 *  
 */
class GLXPlayerLeaderboard : public GLXPlayerWebComponent
{
public:
	/**
	* GLXPlayerLeaderboard constructor.
	* Create an instance of GLXPlayerLeaderboard.
	*/
	GLXPlayerLeaderboard();
	
	/**
	* GLXPlayerLeaderboard destructor.
	* Destroy an instance of GLXPlayerLeaderboard.
	*/
	virtual ~GLXPlayerLeaderboard();

	/**
	* Function sendHighScore.
	* Uploads one score for one level into the server, if the level has no supplemental data. 
	* The traditional way to send hiscore. Just send single score only.
	* @param score The score to be uploaded.
	* @param level The level number for that score. If the game only has one leaderboard, don't send the level parameter and call this function with level = -1.
	* @param scoreType  Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.
	* @return Success: |r|s|current ranking / Fail:  |r|e|error code
	*/	
	bool sendHighScore(int score, int level, int scoreType = SCORE_TYPE_POINTS);	
	
	/**
	* Function addScoreEntry.
	* Add an entry into score array.
	*  By using an score array, user can upload multiple score with or without supplemental data. 
	* @param scoreArray The score array to be uploaded.
	* @param score The score to be uploaded.
	* @param level The level number for that score. If the game only has one leaderboard, don't send the level parameter and call this function with level = -1.
	* @param scoreType  Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
	*/	
	void addScoreEntry(char * scoreArray, int score, int level, int scoreType =  SCORE_TYPE_POINTS, int* supplemental_data = NULL, int supplemental_data_len = 0);					
	
	/**
	* Function sendHighScore.
	* Uploads score into the server. 
	* By using an score array, user can upload multiple score with or without supplemental data.  
	* @param scoreArray The score to be uploaded.	
	* @return Success: |r|s|current ranking / Fail:  |r|e|error code
	*/
	bool sendHighScore(char * scoreArray);		
	
	/**
	* Function sendRankGet.
	* Starts a request for one leaderboard. 
	* @param level The level to get the leaderboard for.
	* @param number_of_players The maximum number of players in the leaderboard the server should return, starting with the highest ranked player.			
	* @param score_type Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
	* @return Success: |r|s|leader board data / Fail:  |r|e|error code
	*/
	bool sendRankGet(int level, int start_pos = 0, int number_of_players = 10, int score_type = SCORE_TYPE_POINTS, bool isGetByCountry = true);	

	/**
	* Function sendRankGetAroundPlayer.
	* Starts a request for getting the scores of players ranked around the user's current standing in a leaderboard. 
	* @param level The level to get the leaderboard for.
	* @param number_of_players_around The maximum number of players that are around the user's rank in the leaderboard the server should return.
	* @param score_type Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
	* @return Success: |r|s|leader board data / Fail:  |r|e|error code
	*/
	bool sendRankGetAroundPlayer(int level, int number_of_players_around = 5, int scoreType = SCORE_TYPE_POINTS);				
	
	/**
	* Function sendRankGetFriends.
	* Starts a request for getting the scores of friends in a leaderboard. 
	* @param level The level to get the leaderboard for.
	* @param number_of_friends The maximum number of friends that we want to get in the leaderboard the server should return.
	* @param score_type Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
	* @return Success: |r|s|leader board data / Fail:  |r|e|error code
	*/
	bool sendRankGetFriends(int level, int number_of_friends = 5, int scoreType = SCORE_TYPE_POINTS);

	/**
	* Function getLeaderboardEntry.
	* Get all the leaderboard data of one entry indicated by index in a single request.
	* @param index Index of current entry.
	* @param name Name of player.
	* @param position Position of player in the leaderboard.
	* @param score Score of player.
	* @param scoreData The scode datas array to fill.
	* @param numberOfSupplementalData Number of supplemental data in the score.		
	* @return True / False
	*/
	bool getLeaderboardEntry(int index, char* &name, int &position, int &score, int* &scoreData, int &numberOfSupplementalData);
	
	/**
	* Function getLeaderboardEntryPlayerName.
	* Gets the name for the player at index in the server returned.
	* @param index Index of current entry.
	* @return Name of player.
	*/
	char* getLeaderboardEntryPlayerName(int index);

	char *getLeaderboardEntryPlayerCountry(int index);
	
	/**
	* Function getLeaderboardEntryPlayerPosition.
	* Gets the ranking of the player at index in the server returned.
	* @param index Index of current entry.
	* @return Ranking of player.
	*/
	int getLeaderboardEntryPlayerPosition(int index);
	
	/**
	* Function getLeaderboardEntryPlayerScore.
	* Gets the scoreof the player at index in the server returned.
	* @param index Index of current entry.	
	* @return Score of player.
	*/
	int getLeaderboardEntryPlayerScore(int index);
	
	/**
	* Function getLeaderboardEntryPlayerScoreData.
	* Gets the supplemental data for the player at index in the server returned list.
	* @param index Index of current entry.		
	* @return Supplemental data array.
	*/
	int* getLeaderboardEntryPlayerScoreData(int index);
	
	/**
	* Function getLeaderboardSize.
	*  Gets the leaderboard size for the currently accessed leaderboard list.		
	* @return Size of leader board.
	*/
	int getLeaderboardSize();
	
	/**
	* Function getCurrentPlayerLeaderboardPosition.
	*  Gets this user's position from the currently accessed leaderboard.		
	* @return Current user's ranking.
	*/
	int getCurrentPlayerLeaderboardPosition();

	char *getCurrentPlayerLeaderboardCountry();
	
	/**
	* Function getCurrentPlayerLeaderboardScore.
	*  Gets this user's score from the currently accessed leaderboard.	
	* @return Current user's score.
	*/
	int getCurrentPlayerLeaderboardScore();
	
	/**
	* Function getCurrentPlayerLeaderboardScoreData.
	*  Gets this user's score supplemental data from the currently accessed leaderboard.
	* @return Current user'ssupplement data array.
	*/
	int* getCurrentPlayerLeaderboardScoreData();						
	
protected:
	/**
	* Private function processRanking.
	* Process leader board data if success message returned from the server.
	* @see sendRankGet()	
	* @param leaderboardData Data returned from the server.
	*/
	void processRanking(char *leaderboardData);
	
	/**
	* Private function processRankingAround.
	* Process leader board data around player if success message returned from the server.
	* @see sendRankGetAroundPlayer()	
	* @param leaderboardData Data returned from the server.
	*/
	void processRankingAround(char *leaderboardData);
	
	/**
	* Private function processRankingFriends.
	* Process leader board data if success message returned from the server.
	* @see sendRankGetFriends()	
	* @param leaderboardData Data returned from the server.
	*/
	void processRankingFriends(char *leaderboardData);

	/**
	* Private function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool OnUpdateSuccess( int func_id );

private:
	/**
	* Private function clearLeaderboard.
	* Clear  leader board data.	
	*/
	void clearLeaderboard();
	
	/**
	* Variable leaderboardSize.
	* Store the size of the leaderboard 
	*/	
	int leaderboardSize;	
	
	/**
	* Variable leaderboardPlayerNames.
	* Array of names for the players in the leaderboard
	*/
	char** leaderboardPlayerNames;	

	char **leaderboardPlayerCountries;
	
	/**
	* Variable leaderboardPlayerPositions.
	* Array of players position in the leaderboard
	*/
	int* leaderboardPlayerPositions;	
	
	/**
	* Variable leaderboardPlayerScores.
	* Array of scores for the players in the leaderboard
	*/
	int* leaderboardPlayerScores;	
	
	/**
	* Variable leaderboardPlayerScoreDatas.
	* 2-dimensional array of score data for the players in the leaderboard.
	* There is one entry in the array for each player, each entry containing leaderboardSupplementalDataFieldsNumber values	
	*/
	int** leaderboardPlayerScoreDatas;	
	
	/**
	* Variable leaderboardSupplementalDataFieldsNumber.
	* The number of supplemental data fields in the current leaderboard
	* The caller is responsable for providing this value, otherwise the data processing will fail ...
	*/
	int leaderboardSupplementalDataFieldsNumber;
	
	/**
	* Variable currentPlayerLeaderboardPosition.
	* Current player's ranking	
	*/
	int currentPlayerLeaderboardPosition;
	
	/**
	* Variable currentPlayerLeaderboardPosition.
	* Current player's score	
	*/
	int currentPlayerLeaderboardScore;
	
	/**
	* Variable currentPlayerLeaderboardPosition.
	* Current player's supplemental data.	
	*/
	int* currentPlayerLeaderboardScoreData;

	char *currentPlayerLeaderboardCountry;
};

#endif //__GL_XPLAYER_LEADERBOARD_H__