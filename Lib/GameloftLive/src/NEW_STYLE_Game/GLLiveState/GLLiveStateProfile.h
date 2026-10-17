#pragma once

#include "GLLiveState.h"
#include "GLLiveStateFriends.h"

#define FLY_SPEED              ((float)3)
#define AVATAR_SCALE_RATE      0.7


class AvatarInfo;

class GLLiveStateProfile : public GLLiveState
{
public:

	enum ProfileSubState
	{	
		PROFILE_NULL,
		PROFILE_EDIT,
		PROFILE_SEL_AVATAR,
		PROFILE_SEL_COUNTRY,
		PROFILE_CHANGE_PWD,
		PROFILE_DEL_CONFIRM,
		PROFILE_REPLACE,
		PROFILE_PROCESS,	
		PROFILE_ERROR		
	};

	GLLiveStateProfile(GLLiveStateHandler *handler);
	virtual ~GLLiveStateProfile(void);

	bool HandleEvent( _TOUCH_EVENT event );
	bool HandleEventMain( _TOUCH_EVENT event );
	bool HandleEventSelAvatar( _TOUCH_EVENT event );
	bool HandleEventSelCountry( _TOUCH_EVENT event );
	bool HandleEventChangePwd( _TOUCH_EVENT event );
	bool HandleEventReplace( _TOUCH_EVENT event );

    void SwitchState();

	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_PROFILE;}
	bool IsProcess(){return m_sub_state == PROFILE_PROCESS;}

    void Paint();
	void PaintMain();
	void PaintSelAvatar();
	void PaintSelCountry();
	void PaintChangePwd();
	void PaintDelConfirm();
	void PaintReplace();

	void UpdateAnimation();

	void OnMenuItemClicked( int id );

// GLXPlayerObserver interface implement
	//void OnNetworkError();
	//void OnRequestTimeout( int func_id );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	void OnRequestFailure( int func_id, int error_code );

//  Chat observer interface
	void OnChatConnected();
private:
	bool SendChangeUserInfo();
	bool SendChangeAvatar();
	bool SendChangeUsername();

	bool CheckProfile();

	void PageDown();
	void PageUp();

	void LoadAvatarListFromLocal();
	bool CheckAvatarList();

	void InitSelAvatarData();
	void PaintListAvatar(int index, int x, int y, int scalex = -1, int scaley = -1);      //index is same as m_select

	void SetEmailFlag(bool flag);


	//_TouchRect m_trAvatar;
	//_TouchRect m_trCountry;
	//_TouchRect m_trUsername;
	//_TouchRect m_trEmail;
	//_TouchRect m_trPassword;
	//_TouchRect m_trEmailFlag;
	//_TouchRect m_trSelector;
	_TouchRect m_trSelAvatar;
	_TouchRect m_trSelCountry;

	//GLLiveMenu
	GLLiveMenu     * m_menu;
	GLLiveMenu     * m_pwdMenu;
	GLLiveMenu     * m_confirmMenu;

	//Profile
	//AvatarInfo *m_avatar;

	int        m_country; //index
	char       m_avatarId[32];  //id
	int        m_avatar;        //Index
	char       m_email[MAX_EMAIL];
	//char       m_password[MAX_PASSWORD];
	char       m_username[MAX_USERNAME];
	bool       m_emailFlag;

	char       m_currentPwd[MAX_PASSWORD];
	char       m_newPwd[MAX_PASSWORD];
	char       m_confirmPwd[MAX_PASSWORD];

	bool       m_changedPwd;
	bool       m_nameChanged;

	ProfileSubState m_sub_state;
	ProfileSubState m_last_state;

	//Animation data
	int        m_offset;
	int        m_oldOffset;
	int        m_select;           //0-choose icon 1 - avatar 0..., save as m_selectTemp
	int        m_startX;
	int        m_startY;
	int        m_viewWidth;
	int        m_pageWidth;
	int        m_totalWidth;
	int        m_maxElement;
	int        m_elementWidth;
	int        m_separateWidth;
	int        m_touchPos;         //X-
	bool       m_moving;
	bool       m_moved;
	int        m_totalPage;
	int        m_curPage;
	int        m_pageX;
	int        m_pageY;
	int        m_holdCount;
	int        m_speed;
	int        m_pageElement;

	//For flying animation data
	float      m_flyX;
	float      m_flyY;
	float      m_distanceX;
	float      m_distanceY;
	bool       m_flying;
	int        m_selectTemp;
	float      m_scaleRate;
	bool       m_scaling;
	float      m_initScaleRate;

	bool       m_pressLeft;
	bool       m_pressRight;
	bool       m_isDeleting;
	float      m_rotateAngle;
	bool       m_rotateLeft;
	bool       m_cancelDeleting;
	bool       m_pressAvatar;
	int        m_pressAvatarCount;

	CGLImage  **m_pAvatarList;
	int        m_avatarCount;
	int        m_downloadAvatarIndex;
	
	bool       m_isUploadAvatar;
	char       m_selectAvatarId[32];

	int        m_replaceX;
	int        m_replaceY;
};