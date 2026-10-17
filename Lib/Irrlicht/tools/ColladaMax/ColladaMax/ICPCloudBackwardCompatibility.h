#ifndef _IC_PCLOUD_BACKWARD_COMPATIBLITY_H
#define _IC_PCLOUD_BACKWARD_COMPATIBLITY_H

class ICPCloudBackwardCompatiblity
{
public:
	static const int k_previousVersionCount;
	static ParamVersionDesc k_previousVersionDescriptions[];
	static ParamBlockDescID k_previousVersion_00[];

	static const int k_currentVersionNumber;
	static const int k_currentVersionParamCount;
	static ParamVersionDesc k_currentVersionDescription;
	static ParamBlockDescID k_currentVersion[];
};

#endif