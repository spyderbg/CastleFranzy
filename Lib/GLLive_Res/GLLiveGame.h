#ifndef _GLLIVE_H_
#define _GLLIVE_H_

class CGLLive
{
public:
	CGLLive(void* app = NULL, void* window = NULL, void* view = NULL, int lang = 0, char* deviceToken = NULL, bool canEnterLeaderboard = false);
	~CGLLive(void);

	bool Update();
	void Paint();
	void handleTouch(float x, float y, bool pressed);
	void SetOrientation(int type);//0: landscapeLeft, 1:landscapeRight, GLLive only suport the two
	void SetInviteToPlay(bool enable);// if there is on MP in your game, set enable = false; the default is true
	//void Pause();
	//void Resume();


	//Interface for set local trophy data
	static bool NotifyTrophy(int trophyIndex);
	static bool* LoadTrophyData(int userno);
	static bool BindTrophyAccount(int userno);
	static int  ReadTrophyAccount();
	static bool ReadUserInfo(char *username, int userLen, char *password, int pwdLen, int *timestamp = NULL);
	static bool SaveUserInfo(bool autoLogin, const char *username, const char *password, int stamp = 0);
private:
	char ForOriginalSizeOfCGLLive[1500];//1448
};

#endif
