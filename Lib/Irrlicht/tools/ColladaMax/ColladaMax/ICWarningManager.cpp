/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ICWarningManager.h"
#include "resource.h"
#include "ColladaOptions.h"
#include "IMorpher/IMorpher.h"
#include "FCDocument/FCDEntity.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometryPolygons.h"
#include "FCDocument/FCDGeometryPolygonsInput.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDMorphController.h"
#include "FCDocument/FCDSceneNode.h"

ICWarningManager::ICWarningManager()
{
	Reset();
}

ICWarningManager::~ICWarningManager()
{

}

void ICWarningManager::Reset()
{
	m_warnings.clear();

	m_warnings.push_back( ICWarning( ICWarning::NEGATIVE_SCALE, "warnOnNegativeScale", "WarnOnNegativeScale", IDC_WARN_NEGSCALE, ICWarningFlags::ENABLED | ICWarningFlags::COMPLY_CONVERTER, &Log_WarnOnNegativeScale ));
	m_warnings.push_back( ICWarning( ICWarning::INCOMPATIBLE_MORPH_TARGET, "warnOnIncompatibleMorphTarget", "WarnOnIncompatibleMorphTarget", IDC_WARN_INCOMPATIBLE_MORPH_TARGET, ICWarningFlags::ENABLED | ICWarningFlags::COMPLY_CONVERTER, &Log_WarnOnIncompatibleMorphTarget ));
	m_warnings.push_back( ICWarning( ICWarning::INVALID_GEOMETRY, "warnOnInvalidGeometry", "WarnOnInvalidGeometry", IDC_WARN_INVALID_GEOMETRY, ICWarningFlags::ENABLED, &Log_WarnOnInvalidGeometry ));
	m_warnings.push_back( ICWarning( ICWarning::INVALID_SKIN, "warnOnInvalidSkin", "WarnOnInvalidSkin", IDC_WARN_INVALID_SKIN, ICWarningFlags::ENABLED, &Log_WarnOnInvalidSkin ));
}

ICWarning *ICWarningManager::GetWarningById( int id )
{
	ICWarning *pWarning = NULL;

	for ( fm::vector<ICWarning>::iterator warningIter = m_warnings.begin(); warningIter != m_warnings.end(); ++warningIter )
	{
		if ( (*warningIter).m_id == id )
		{
			pWarning = &m_warnings[id];
			break;
		}
	}

	return pWarning;
}

void ICWarningManager::SetWarningValuesFromWidgets( HWND hWnd )
{
	for ( fm::vector<ICWarning>::iterator warningIter = m_warnings.begin(); warningIter != m_warnings.end(); ++warningIter )
	{
		ICWarning *pWarning = ( ICWarning * )warningIter;
		RESET_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED );
		SET_FLAG( pWarning->m_flags, IsDlgButtonChecked(hWnd, pWarning->m_idc ) == BST_CHECKED ? ICWarningFlags::ENABLED : 0 );
	}
}

void ICWarningManager::SetWidgetsFromWarningValues( HWND hWnd )
{
	for ( fm::vector<ICWarning>::iterator warningIter = m_warnings.begin(); warningIter != m_warnings.end(); ++warningIter )
	{
		ICWarning *pWarning = ( ICWarning * )warningIter;
		CheckDlgButton(hWnd, pWarning->m_idc, TEST_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED ));
	}
}

bool ICWarningManager::IsWarningEnabled( int id )
{
	ICWarning *pWarning = GetWarningById( id );
	return pWarning && TEST_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED );
}

// log funcs
extern FUStringBuilder g_warningLog;

void ICWarningManager::Log_WarnOnNegativeScale( void *param, ... )
{
	va_list args;
	va_start(args, param);

	FCDSceneNode *colladaNode = va_arg(args, FCDSceneNode *);
	Point3 &k = va_arg( args, Point3 );

	int numNegElements = ( k.x < 0 ? 1 : 0 )
		+ ( k.y < 0 ? 1 : 0 )
		+ ( k.z < 0 ? 1 : 0 );

	if ( numNegElements % 2 == 1 )
	{
		// we have an invalid negative scale
		g_warningLog.append( "Warning! Invalid negative scale" );
		g_warningLog.append( " (" );
		g_warningLog.append( FMVector3( k.x, k.y, k.z ) );
		g_warningLog.append( " ) " );
		g_warningLog.append( "@ node " );
		g_warningLog.append( "'" );
		g_warningLog.append( colladaNode->GetName() );
		g_warningLog.append( "'" );
		g_warningLog.append( "\n" );
	}

	va_end(args);
}

#define MORPH_TARGET_WARNING( log, msg, targetName, baseName ) \
{ \
	log.append( "Warning! Incompatible morph target " ); \
	log.append( "'" ); \
	log.append( targetName ); \
	log.append( "'" ); \
	log.append( " @ node " ); \
	log.append( "'" ); \
	log.append( baseName ); \
	log.append( "'" ); \
	log.append( " ( " ); \
	log.append( msg ); \
	log.append( " ) " ); \
	log.append( "\n" ); \
}

#define GET_BASE_GEOMETRY( entity )(( entity && entity->GetType() == FCDEntity::CONTROLLER ) ? ((FCDController*) entity)->GetBaseGeometry() : entity )

void ICWarningManager::Log_WarnOnIncompatibleMorphTarget( void *param, ... )
{
	va_list args;
	va_start(args, param);

	int testId = *(int*)param;
	if ( testId == MISSING_TARGET_TEST )
	{
		INode *node = va_arg( args, INode * );
		morphChannel *channel = va_arg( args, morphChannel * );
		INode *targetNode = channel->mConnection;

		if ( !targetNode )
		{
			MORPH_TARGET_WARNING( g_warningLog, "missing target object", channel->mName.data(), node->GetName() );
			va_end(args);
			return;
		}
	}
	else if ( testId == TARGET_DIFFERENCES_TEST )
	{
		FCDEntity *baseTarget = va_arg( args, FCDEntity * );
		FCDGeometry *targetMesh = va_arg( args, FCDGeometry * );
		FCDMorphTarget *target = va_arg( args, FCDMorphTarget * );

		FCDEntity* baseEntity = GET_BASE_GEOMETRY( baseTarget );
		FUAssert( baseEntity->GetType() == FCDEntity::GEOMETRY, va_end(args); return );
		FUAssert( targetMesh, va_end(args); return );

		if ( target == NULL )
		{
			MORPH_TARGET_WARNING( g_warningLog, "different number of vertices", targetMesh->GetName(), baseEntity->GetName() );
			va_end(args);
			return;
		}

		FCDGeometryMesh *pBaseTargetGeometryMesh = (( FCDGeometry * )baseEntity)->GetMesh();
		size_t baseTargetPolygonsCount = pBaseTargetGeometryMesh->GetPolygonsCount();
		
		FCDGeometryMesh *pTargetGeometryMesh = targetMesh->GetMesh();
		size_t targetPolygonsCount = pTargetGeometryMesh->GetPolygonsCount();
		
		if ( baseTargetPolygonsCount != targetPolygonsCount )
		{
			MORPH_TARGET_WARNING( g_warningLog, "different number of materials", targetMesh->GetName(), baseEntity->GetName() );
		}
	}

	va_end(args);
}

void ICWarningManager::Log_WarnOnInvalidGeometry( void *param, ... )
{
	va_list args;
	va_start(args, param);

	FCDGeometryMesh *colladaMesh = va_arg( args, FCDGeometryMesh * );
	INode *pNode = va_arg( args, INode * );
	colladaMesh;

	g_warningLog.append( "Warning!  Invalid geometry " );
	g_warningLog.append( "@ node " );
	g_warningLog.append( "'" );
	g_warningLog.append( pNode->GetName() );
	g_warningLog.append( "'" );
	g_warningLog.append( "\n" );

	va_end(args);
}

void ICWarningManager::Log_WarnOnInvalidSkin( void *param, ... )
{
	va_list args;
	va_start(args, param);

	INode *pNode = va_arg( args, INode * );

	g_warningLog.append( "Warning!  Invalid skin " );
	g_warningLog.append( "@ node " );
	g_warningLog.append( "'" );
	g_warningLog.append( pNode->GetName() );
	g_warningLog.append( "'" );
	g_warningLog.append( "\n" );

	va_end(args);
}

#undef MORPH_TARGET_WARNING
#undef GET_BASE_GEOMETRY