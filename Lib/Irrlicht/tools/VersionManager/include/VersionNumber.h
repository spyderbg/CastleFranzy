#pragma once

#include <string>

#define INV_MAJOR -1
#define INV_MINOR -1
#define INV_BUILD -1
#define INV_REV -1

#define MAX_VERNUM_COMPONENT 65535
#define NUM_VERNUM_COMPONENTS 4

class VersionNumber
{
private:
	VersionNumber &Increment();
	static char GetSeparator( const char *verStr );
public:
	int m_major;
	int m_minor;
	int m_build;
	int m_revision;

	VersionNumber( int major = 0, int minor = 0, int build = 0, int revision = 0 );
	static VersionNumber Invalid();

	VersionNumber &operator++();
	VersionNumber operator++(int);

	bool operator==(VersionNumber verNum) const;
	bool operator!=(VersionNumber verNum) const;
	bool operator>(VersionNumber verNum) const;
	bool operator<(VersionNumber verNum) const;
	bool operator<=(VersionNumber verNum) const;
	bool operator>=(VersionNumber verNum) const;

	// expects a string in this format : "major.minor.build.revision"
	static VersionNumber Get( char *verStr );
	static VersionNumber Get( const char *verStr );

	// caused an unresolved external issue when linking with this lib from a VC2005 project.
	//std::string ToString() const;
};