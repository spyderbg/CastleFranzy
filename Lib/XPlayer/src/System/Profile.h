#ifndef __PROFILE_H__
#define __PROFILE_H__

#if IPHONE
#define PROFILE_ENABLE	0
#else
#define PROFILE_ENABLE	1
#endif

#define MAX_PROFILES	256

typedef struct 
{
	char			_name[256];
	unsigned int	_time1;
	unsigned int	_time2;
}ProfileInfo;

#if PROFILE_ENABLE
class CProfile
{
public:
	CProfile(void);
	~CProfile(void);

	void Clear();
	void Begin(const char* text);
	void End(const char* text = NULL);
	inline const int Count() const{return m_nCount;};
	inline const ProfileInfo& operator[](int idx) const{ASSERT(idx >= 0 && idx < m_nCount);return m_profiles[idx];};

private:
	ProfileInfo		m_profiles[MAX_PROFILES];
	int				m_nCount;
};

#else

class CProfile
{
public:
	CProfile(void){memset(m_profiles, 0, sizeof(m_profiles));};
	~CProfile(void){};

	void Clear(){};
	void Begin(const char* text){};
	void End(const char* text = NULL){};
	inline const int Count() const{return 0;};
	inline const ProfileInfo& operator[](int idx) const{return *(ProfileInfo*)NULL;};

private:
	ProfileInfo		m_profiles[MAX_PROFILES];
	int				m_nCount;
};
#endif

#endif