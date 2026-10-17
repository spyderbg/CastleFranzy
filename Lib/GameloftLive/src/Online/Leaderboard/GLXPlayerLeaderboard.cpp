#include "GLXPlayerLeaderboard.h"

namespace gllive
{
	/**
	* GLXPlayerLeaderboard constructor.
	* Create an instance of GLXPlayerLeaderboard.
	*/
	GLXPlayerLeaderboard::GLXPlayerLeaderboard() : GLXPlayerWebComponent()
	{
		leaderboardPlayerNames				= NULL;
		leaderboardPlayerPositions			= NULL;
		leaderboardPlayerScores				= NULL;
		leaderboardPlayerScoreDatas			= NULL;
		currentPlayerLeaderboardScoreData	= NULL;
		leaderboardSize						= NOT_A_NUMBER;
		currentPlayerLeaderboardPosition	= NOT_A_NUMBER;
		currentPlayerLeaderboardScore		= NOT_A_NUMBER;
	}

	/**
	* GLXPlayerLeaderboard destructor.
	* Destroy an instance of GLXPlayerLeaderboard.
	*/
	GLXPlayerLeaderboard::~GLXPlayerLeaderboard()
	{
		clearLeaderboard();	
	}

	/**
	* Private function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool GLXPlayerLeaderboard::OnUpdateSuccess( int func_id )
	{		
		switch(func_id)
		{
			case FUNCTION_GET_RANKINGS:
				clearLeaderboard();			
				leaderboardSupplementalDataFieldsNumber = 0;
				processRanking(m_pResponsePtr);						
				break;
			case FUNCTION_GET_RANKINGS_AROUND_PLAYER:
				clearLeaderboard();			
				leaderboardSupplementalDataFieldsNumber = 0;
				processRankingAround(m_pResponsePtr);			
				break;
			case FUNCTION_GET_RANKINGS_FRIENDS:
				clearLeaderboard();			
				leaderboardSupplementalDataFieldsNumber = 0;
				processRankingFriends(m_pResponsePtr);	
				break;
			case FUNCTION_UPLOAD_SCORE:
				{
					char tmp[32] = {0};
					getValue(m_pResponsePtr,tmp, 3, '|');
					currentPlayerLeaderboardPosition = XP_API_ATOI(tmp);				
				}	
				break;
				
			default:			
				break;
		}
		return GLXPlayerWebComponent::OnUpdateSuccess( func_id );	
	}

	/**
	* Function sendHighScore.
	* Uploads one score for one level into the server, if the level has no supplemental data. 
	* The traditional way to send hiscore. Just send single score only.
	* @param score The score to be uploaded.
	* @param level The level number for that score. If the game only has one leaderboard, don't send the level parameter and call this function with level = -1.
	* @param scoreType  Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.
	* @return Success: |r|s|current ranking / Fail:  |r|e|error code
	*/
	bool GLXPlayerLeaderboard::sendHighScore(int score, int level, int scoreType)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		if (level >= 0)
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|l|%d|t|%d|s|%d|", FUNCTION_UPLOAD_SCORE, m_ggi, m_uid, level, scoreType, score);
		}
		else
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|t|%d|s|%d|", FUNCTION_UPLOAD_SCORE, m_ggi, m_uid, scoreType, score);
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendHighscore before String2Blob -> buffer = %s\n", buffer);
		
		return SendByGet( buffer );
	}

	/**
	* Function addScoreEntry.
	* Add an entry into score array.
	*  By using an score array, user can upload multiple score with or without supplemental data. 
	* @param scoreArray The score array to be uploaded.
	* @param score The score to be uploaded.
	* @param level The level number for that score. If the game only has one leaderboard, don't send the level parameter and call this function with level = -1.
	* @param scoreType  Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
	*/
	void GLXPlayerLeaderboard::addScoreEntry(char * scoreArray, int score, int level, int scoreType, int* supplemental_data, int supplemental_data_len)
	{
		XP_API_SPRINTF( scoreArray + XP_API_STRLEN(scoreArray), 
			"l|%d|t|%d|sl|0|s|%d|", level, scoreType, score);

		for (int i = 0; i < supplemental_data_len; i++)
		{
			XP_API_SPRINTF(scoreArray + XP_API_STRLEN(scoreArray), 
				"l|%d|t|%d|sl|%d|s|%d|", level, scoreType, i + 1, supplemental_data[i]);
		}
	}

	/**
	* Function sendHighScore.
	* Uploads score into the server. 
	* By using an score array, user can upload multiple score with or without supplemental data.  
	* @param scoreArray The score to be uploaded.	
	* @return Success: |r|s|current ranking / Fail:  |r|e|error code
	*/
	bool GLXPlayerLeaderboard::sendHighScore(char * scoreArray)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE ];	
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_UPLOAD_SCORE, m_ggi, m_uid);
		XP_API_SPRINTF(buffer + XP_API_STRLEN(buffer), scoreArray);
		
		XP_DEBUG_OUT("GLXPlayerUser::sendMultipleHighscores before String2Blob -> buffer = %s\n", buffer);

		return SendByGet( buffer );
	}

	/**
	* Function sendRankGet.
	* Starts a request for one leaderboard. 
	* @param level The level to get the leaderboard for.
	* @param number_of_players The maximum number of players in the leaderboard the server should return, starting with the highest ranked player.			
	* @param score_type Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
	* @return Success: |r|s|leader board data / Fail:  |r|e|error code
	*/
	bool GLXPlayerLeaderboard::sendRankGet(int level, int number_of_players, int score_type)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%d|t|%d|", FUNCTION_GET_RANKINGS, m_ggi, m_uid, number_of_players,score_type);

	   if (level >= 0)
		{
			XP_API_SPRINTF(buffer+XP_API_STRLEN(buffer), "l|%d|", level);
		}

		XP_DEBUG_OUT("GLXPlayerUser::sendRankGet before String2Blob -> buffer = %s\n", buffer);
		
		return SendByGet( buffer );
	}

	/**
	* Function sendRankGetAroundPlayer.
	* Starts a request for getting the scores of players ranked around the user's current standing in a leaderboard. 
	* @param level The level to get the leaderboard for.
	* @param number_of_players_around The maximum number of players that are around the user's rank in the leaderboard the server should return.
	* @param score_type Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
	* @return Success: |r|s|leader board data / Fail:  |r|e|error code
	*/
	bool GLXPlayerLeaderboard::sendRankGetAroundPlayer(int level, int number_of_players_around, int scoreType)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE);
		XP_API_SPRINTF(buffer,"f|%d|i|%d|u|%s|p|%d|t|%d|", FUNCTION_GET_RANKINGS_AROUND_PLAYER, m_ggi, m_uid, number_of_players_around, scoreType);

		if (level >= 0)
		{
			XP_API_SPRINTF(buffer+XP_API_STRLEN(buffer), "l|%d|", level);
		}

		XP_DEBUG_OUT("GLXPlayerUser::sendRankGetAroundPlayer before String2Blob -> buffer = %s\n", buffer);
		
		return SendByGet( buffer );
	} 

	/**
		* Function sendRankGetFriends.
		* Starts a request for getting the scores of friends in a leaderboard. 
		* @param level The level to get the leaderboard for.
		* @param number_of_friends The maximum number of friends that we want to get in the leaderboard the server should return.
		* @param score_type Either SCORE_TYPE_POINTS or SCORE_TYPE_TIME, depending on the type of score.	
		* @return Success: |r|s|leader board data / Fail:  |r|e|error code
		*/
	bool GLXPlayerLeaderboard::sendRankGetFriends(int level, int number_of_friends, int scoreType)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%d|t|%d|", FUNCTION_GET_RANKINGS, m_ggi, m_uid, number_of_friends,scoreType);

		if (level >= 0)
		{
			XP_API_SPRINTF(buffer+XP_API_STRLEN(buffer), "l|%d|", level);
		}

		XP_DEBUG_OUT("GLXPlayerUser::sendRankGet before String2Blob -> buffer = %s\n", buffer);
		
		return SendByGet( buffer );
	}

	/**
	* Private function clearLeaderboard.
	* Clear  leader board data.	
	*/
	void GLXPlayerLeaderboard::clearLeaderboard()
	{
		XP_API_DEL_ARRAY(leaderboardPlayerNames,leaderboardSize);
		leaderboardPlayerNames = NULL;

		if (leaderboardSize != 0 && leaderboardSize != NOT_A_NUMBER)
		{
			for (int i = 0; i < leaderboardSize; i++)
			{
				if (leaderboardPlayerScoreDatas)
				{
					XP_API_DEL(leaderboardPlayerScoreDatas[i]);
				}
			}

			XP_API_DEL(leaderboardPlayerScoreDatas);
			leaderboardPlayerScoreDatas = NULL;
		}

		XP_API_DEL(leaderboardPlayerPositions);
		leaderboardPlayerPositions = NULL;

		XP_API_DEL(leaderboardPlayerScores);
		leaderboardPlayerScores = NULL;

		XP_API_DEL(currentPlayerLeaderboardScoreData);
		currentPlayerLeaderboardScoreData = NULL;

		leaderboardSize = NOT_A_NUMBER;
		currentPlayerLeaderboardPosition = NOT_A_NUMBER;
		currentPlayerLeaderboardScore = NOT_A_NUMBER;
	}

	/**
	* Function getCurrentPlayerLeaderboardPosition.
	*  Gets this user's position from the currently accessed leaderboard.		
	* @return Current user's ranking.
	*/
	int GLXPlayerLeaderboard::getCurrentPlayerLeaderboardPosition()
	{
		return currentPlayerLeaderboardPosition;
	}

	/**
	* Function getCurrentPlayerLeaderboardScore.
	*  Gets this user's score from the currently accessed leaderboard.	
	* @return Current user's score.
	*/      
	int GLXPlayerLeaderboard::getCurrentPlayerLeaderboardScore()
	{
		return currentPlayerLeaderboardScore;
	}

	/**
	* Function getCurrentPlayerLeaderboardScoreData.
	*  Gets this user's score supplemental data from the currently accessed leaderboard.
	* @return Current user'ssupplement data array.
	*/
	int* GLXPlayerLeaderboard::getCurrentPlayerLeaderboardScoreData()
	{
		return currentPlayerLeaderboardScoreData;
	}

	/**
	* Function getLeaderboardSize.
	*  Gets the leaderboard size for the currently accessed leaderboard list.		
	* @return Size of leader board.
	*/
	int GLXPlayerLeaderboard::getLeaderboardSize()
	{
		if (leaderboardPlayerNames == NULL)
		{
			return -1;
		}

		return leaderboardSize;
	}

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
	bool GLXPlayerLeaderboard::getLeaderboardEntry(int index,char* &name,int &position,int &score, int* &scoreData,int &numberOfSupplementalData)
	{
		if ( (index >= leaderboardSize) || (leaderboardPlayerNames == NULL))
		{
			return false;
		}

		name = leaderboardPlayerNames[index];
		position = leaderboardPlayerPositions[index];
		score = leaderboardPlayerScores[index];

		if (leaderboardSupplementalDataFieldsNumber > 0)
		{
			scoreData = leaderboardPlayerScoreDatas[index];
		}

		numberOfSupplementalData = leaderboardSupplementalDataFieldsNumber;
		return true;
	}

	/**
	* Function getLeaderboardEntryPlayerName.
	* Gets the name for the player at index in the server returned.
	* @param index Index of current entry.
	* @return Name of player.
	*/
	char* GLXPlayerLeaderboard::getLeaderboardEntryPlayerName(int index)
	{
		if ( (index >= leaderboardSize) || (leaderboardPlayerNames == NULL))
		{
		  return NULL;
		}

		return leaderboardPlayerNames[index];
	}

	/**
	* Function getLeaderboardEntryPlayerPosition.
	* Gets the ranking of the player at index in the server returned.
	* @param index Index of current entry.
	* @return Ranking of player.
	*/
	int GLXPlayerLeaderboard::getLeaderboardEntryPlayerPosition(int index)
	{
		if ( (index >= leaderboardSize) || (leaderboardPlayerPositions == NULL))
		{
		  return -1;
		}

		return leaderboardPlayerPositions[index];
	}

	/**
	* Function getLeaderboardEntryPlayerScore.
	* Gets the scoreof the player at index in the server returned.
	* @param index Index of current entry.	
	* @return Score of player.
	*/
	int GLXPlayerLeaderboard::getLeaderboardEntryPlayerScore(int index)
	{
		if ( (index >= leaderboardSize) || (leaderboardPlayerScores == NULL))
		{
		  return NOT_A_NUMBER;
		}

		return leaderboardPlayerScores[index];
	}

	/**
	* Function getLeaderboardEntryPlayerScoreData.
	* Gets the supplemental data for the player at index in the server returned list.
	* @param index Index of current entry.		
	* @return Supplemental data array.
	*/
	int* GLXPlayerLeaderboard::getLeaderboardEntryPlayerScoreData(int index)
	{
		if ( (index >= leaderboardSize) || (leaderboardPlayerScoreDatas == NULL))
		{
		  return NULL;
		}

		return leaderboardPlayerScoreDatas[index];
	}

	/**
	* Private function processRanking.
	* Process leader board data if success message returned from the server.
	* @see sendRankGet()	
	* @param leaderboardData Data returned from the server.
	*/
	void GLXPlayerLeaderboard::processRanking(char *leaderboardData)
	{
		char tmpHR[64];
		int index = 0;
		getValue(leaderboardData, tmpHR, index++, '|');
		int count = 0;

		if (XP_API_STRCMP(tmpHR, "y") == 0)
		{
			// we got data for ourselvs !!
			// position ..
			getValue(leaderboardData, tmpHR, index++, '|');
			currentPlayerLeaderboardPosition = XP_API_ATOI(tmpHR);

			if (currentPlayerLeaderboardPosition < 0)
			{
				// invalid data for current player ..
				currentPlayerLeaderboardPosition	= NOT_A_NUMBER;
				currentPlayerLeaderboardPosition	= NOT_A_NUMBER;
				currentPlayerLeaderboardScore		= NOT_A_NUMBER;
				currentPlayerLeaderboardScoreData	= NULL;

				// skip the rest of the data for ourselvs ..
				index += 2 + leaderboardSupplementalDataFieldsNumber;
				count -= (3 + leaderboardSupplementalDataFieldsNumber);
			}
			else
			{
				// skip the name ..
				index++;

				// score ...
				getValue(leaderboardData, tmpHR, index++, '|');
				currentPlayerLeaderboardScore = XP_API_ATOI(tmpHR);

				if (leaderboardSupplementalDataFieldsNumber > 0)
				{
					currentPlayerLeaderboardScoreData = XP_API_NEW int[leaderboardSupplementalDataFieldsNumber];
				}
				// score data
				for (int i  = 0 ; i < leaderboardSupplementalDataFieldsNumber ; i++)
				{
					getValue(leaderboardData, tmpHR, index++, '|');
					currentPlayerLeaderboardScoreData[i] = XP_API_ATOI(tmpHR);
				}
			}
		}
		else if (XP_API_STRCMP(tmpHR, "n") != 0)
		{ // the data is not correct .. return
			return;
		}

		// we must calculate how much space we need to store all the leaderboard ...
		// first, count all the "|" in the string   
		for (int i = 0; i < XP_API_STRLEN(leaderboardData);i++)
		{
			if (leaderboardData[i] == '|')
			count++;
		}  

		// if data for ourselvs is present, don't count it in .. that is another 4 '|'
		if (currentPlayerLeaderboardPosition >= 0)
		{
			count -= (3 + leaderboardSupplementalDataFieldsNumber);
		}

		// the data comes in sets of 4 .. divide by 4 then
		leaderboardSize = count / (3 + leaderboardSupplementalDataFieldsNumber);

		// init all the arrays ...
		if (leaderboardSize <= 0)
		{
			return;
		}

		leaderboardPlayerNames = XP_API_NEW char*[leaderboardSize];
		for (int i = 0; i < leaderboardSize; i++)
		{
			leaderboardPlayerNames[i] = 0;
		}
		leaderboardPlayerPositions  = XP_API_NEW int[leaderboardSize];

		leaderboardPlayerScores	   = XP_API_NEW int[leaderboardSize];

		//-- rov    
		if (leaderboardSupplementalDataFieldsNumber > 0)
		{
			leaderboardPlayerScoreDatas = XP_API_NEW int*[leaderboardSize];

			for (int i = 0; i < leaderboardSize; i++)
			{
				leaderboardPlayerScoreDatas[i] = 0;
			}
		}

		// parse the data and fill the arrays ..
		for (int i = 0; i < leaderboardSize; i++)
		{
			// position ..
			getValue(leaderboardData, tmpHR, index++, '|');
			leaderboardPlayerPositions[i] = XP_API_ATOI(tmpHR);

			// name
			getValue(leaderboardData, tmpHR, index++, '|');
			leaderboardPlayerNames[i] = XP_API_NEW char[XP_API_STRLEN(tmpHR)+1];
			XP_API_STRCPY(leaderboardPlayerNames[i], tmpHR);

			// score ...
			getValue(leaderboardData, tmpHR, index++, '|');
			leaderboardPlayerScores[i] = XP_API_ATOI(tmpHR);

			if (leaderboardSupplementalDataFieldsNumber > 0)
			{
				// supplemental
				leaderboardPlayerScoreDatas[i] = XP_API_NEW int[leaderboardSupplementalDataFieldsNumber];

				for (int j  = 0 ; j < leaderboardSupplementalDataFieldsNumber ; j++)
				{
					getValue(leaderboardData, tmpHR, index++, '|');
					leaderboardPlayerScoreDatas[i][j] = XP_API_ATOI(tmpHR);
				}
			}
		}
	}

	/**
	* Private function processRankingAround.
	* Process leader board data around player if success message returned from the server.
	* @see sendRankGetAroundPlayer()	
	* @param leaderboardData Data returned from the server.
	*/
	void GLXPlayerLeaderboard::processRankingAround(char *leaderboardData)
	{  
			int count = 0, index = 0;
			char tmpHR[64];
			// we must calculate how much space we need to store all the leaderboard ...
			// first, count all the "|" in the string   
			for (int i = 0; i < XP_API_STRLEN(leaderboardData);i++)
			{
				if (leaderboardData[i] == '|')
				count++;
			}  

			// if data for ourselvs is present, don't count it in .. that is another 4 '|'
			if (currentPlayerLeaderboardPosition >= 0)
			{
				count -= (3 + leaderboardSupplementalDataFieldsNumber);
			}

			// the data comes in sets of 4 .. divide by 4 then
			leaderboardSize = count / (3 + leaderboardSupplementalDataFieldsNumber);

			// init all the arrays ...
			if (leaderboardSize <= 0)
			{
				return;
			}

			leaderboardPlayerNames = XP_API_NEW char*[leaderboardSize];
			for (int i = 0; i < leaderboardSize; i++)
			{
				leaderboardPlayerNames[i] = 0;
			}

			leaderboardPlayerPositions  = XP_API_NEW int[leaderboardSize];

			leaderboardPlayerScores	   = XP_API_NEW int[leaderboardSize];

			//-- rov    
			if (leaderboardSupplementalDataFieldsNumber > 0)
			{
				leaderboardPlayerScoreDatas = XP_API_NEW int*[leaderboardSize];

				for (int i = 0; i < leaderboardSize; i++)
				{
					leaderboardPlayerScoreDatas[i] = 0;
				}
			}

			// parse the data and fill the arrays ..
			for (int i = 0; i < leaderboardSize; i++)
			{
				// position ..
				getValue(leaderboardData, tmpHR, index++, '|');
				leaderboardPlayerPositions[i] = XP_API_ATOI(tmpHR);

				// name
				getValue(leaderboardData, tmpHR, index++, '|');
				leaderboardPlayerNames[i] = XP_API_NEW char[XP_API_STRLEN(tmpHR)+1];
				XP_API_STRCPY(leaderboardPlayerNames[i], tmpHR);

				// score ...
				getValue(leaderboardData, tmpHR, index++, '|');
				leaderboardPlayerScores[i] = XP_API_ATOI(tmpHR);

				if (leaderboardSupplementalDataFieldsNumber > 0)
				{
					// supplemental
					leaderboardPlayerScoreDatas[i] = XP_API_NEW int[leaderboardSupplementalDataFieldsNumber];

					for (int j  = 0 ; j < leaderboardSupplementalDataFieldsNumber ; j++)
					{
						getValue(leaderboardData, tmpHR, index++, '|');
						leaderboardPlayerScoreDatas[i][j] = XP_API_ATOI(tmpHR);
					}
				}
		}
	}

	/**
		* Private function processRankingFriends.
		* Process leader board data if success message returned from the server.
		* @see sendRankGetFriends()	
		* @param leaderboardData Data returned from the server.
		*/
	void GLXPlayerLeaderboard::processRankingFriends(char *leaderboardData)
	{
		char tmpHR[64];
		int index = 0;
		getValue(leaderboardData, tmpHR, index++, '|');
		int count = 0;

		if (XP_API_STRCMP(tmpHR, "y") == 0)
		{
			// we got data for ourselvs !!
			// position ..
			getValue(leaderboardData, tmpHR, index++, '|');
			currentPlayerLeaderboardPosition = XP_API_ATOI(tmpHR);

			if (currentPlayerLeaderboardPosition < 0)
			{
				// invalid data for current player ..
				currentPlayerLeaderboardPosition	= NOT_A_NUMBER;
				currentPlayerLeaderboardPosition	= NOT_A_NUMBER;
				currentPlayerLeaderboardScore		= NOT_A_NUMBER;
				currentPlayerLeaderboardScoreData	= NULL;

				// skip the rest of the data for ourselvs ..
				index += 2 + leaderboardSupplementalDataFieldsNumber;
				count -= (3 + leaderboardSupplementalDataFieldsNumber);
			}
			else
			{
				// skip the name ..
				index++;

				// score ...
				getValue(leaderboardData, tmpHR, index++, '|');
				currentPlayerLeaderboardScore = XP_API_ATOI(tmpHR);

				if (leaderboardSupplementalDataFieldsNumber > 0)
				{
					currentPlayerLeaderboardScoreData = XP_API_NEW int[leaderboardSupplementalDataFieldsNumber];
				}
				// score data
				for (int i  = 0 ; i < leaderboardSupplementalDataFieldsNumber ; i++)
				{
					getValue(leaderboardData, tmpHR, index++, '|');
					currentPlayerLeaderboardScoreData[i] = XP_API_ATOI(tmpHR);
				}
			}
		}
		else if (XP_API_STRCMP(tmpHR, "n") != 0)
		{ // the data is not correct .. return
			return;
		}

		// we must calculate how much space we need to store all the leaderboard ...
		// first, count all the "|" in the string   
		for (int i = 0; i < XP_API_STRLEN(leaderboardData);i++)
		{
			if (leaderboardData[i] == '|')
			count++;
		}  

		// if data for ourselvs is present, don't count it in .. that is another 4 '|'
		if (currentPlayerLeaderboardPosition >= 0)
		{
			count -= (3 + leaderboardSupplementalDataFieldsNumber);
		}

		// the data comes in sets of 4 .. divide by 4 then
		leaderboardSize = count / (3 + leaderboardSupplementalDataFieldsNumber);

		// init all the arrays ...
		if (leaderboardSize <= 0)
		{
			return;
		}

		leaderboardPlayerNames = XP_API_NEW char*[leaderboardSize];
		for (int i = 0; i < leaderboardSize; i++)
		{
			leaderboardPlayerNames[i] = 0;
		}
		leaderboardPlayerPositions  = XP_API_NEW int[leaderboardSize];

		leaderboardPlayerScores	   = XP_API_NEW int[leaderboardSize];

		//-- rov    
		if (leaderboardSupplementalDataFieldsNumber > 0)
		{
			leaderboardPlayerScoreDatas = XP_API_NEW int*[leaderboardSize];

			for (int i = 0; i < leaderboardSize; i++)
			{
				leaderboardPlayerScoreDatas[i] = 0;
			}
		}

		// parse the data and fill the arrays ..
		for (int i = 0; i < leaderboardSize; i++)
		{
			// position ..
			getValue(leaderboardData, tmpHR, index++, '|');
			leaderboardPlayerPositions[i] = XP_API_ATOI(tmpHR);

			// name
			getValue(leaderboardData, tmpHR, index++, '|');
			leaderboardPlayerNames[i] = XP_API_NEW char[XP_API_STRLEN(tmpHR)+1];
			XP_API_STRCPY(leaderboardPlayerNames[i], tmpHR);

			// score ...
			getValue(leaderboardData, tmpHR, index++, '|');
			leaderboardPlayerScores[i] = XP_API_ATOI(tmpHR);

			if (leaderboardSupplementalDataFieldsNumber > 0)
			{
				// supplemental
				leaderboardPlayerScoreDatas[i] = XP_API_NEW int[leaderboardSupplementalDataFieldsNumber];

				for (int j  = 0 ; j < leaderboardSupplementalDataFieldsNumber ; j++)
				{
					getValue(leaderboardData, tmpHR, index++, '|');
					leaderboardPlayerScoreDatas[i][j] = XP_API_ATOI(tmpHR);
				}
			}
		}
	}
}