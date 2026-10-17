/* $Id: igd_desc_parse.c,v 1.7 2006/11/19 22:32:33 nanard Exp $ */
/* Project : miniupnp
 * http://miniupnp.free.fr/
 * Author : Thomas Bernard
 * Copyright (c) 2005 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution.
 * 
 */
#include "igd_desc_parse.h"
#include "../../../../../include/Porting.h"

/* Start element handler :
 * update nesting level counter and copy element name */
void IGDstartelt(void * d, const char * name, int l)
{
	struct IGDdatas * datas = (struct IGDdatas *)d;
	XP_API_MEMCPY( datas->cureltname, (char *)name, l);
	datas->cureltname[l] = '\0';
	datas->level++;
}

/* End element handler :
 * update nesting level counter and update parser state if
 * service element is parsed */
void IGDendelt(void * d, const char * name, int l)
{
	struct IGDdatas * datas = (struct IGDdatas *)d;
	datas->level--;
	/*printf("endelt %2d %.*s\n", datas->level, l, name);*/
	if( (l==7) && !XP_API_STRNCMP(name, "service", l) )
	{
		/*datas->state++; */
		/*
		if( datas->state < 1
			&& !strcmp(datas->servicetype,
				//	"urn:schemas-upnp-org:service:WANIPConnection:1") )
				"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1"))
			datas->state ++;
		*/
		if(0==XP_API_STRCMP(datas->servicetype_CIF,
				"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1"))
			datas->state = 2;
		if(0==XP_API_STRCMP(datas->servicetype,
				"urn:schemas-upnp-org:service:WANIPConnection:1") )
			datas->state = 3;
/*		if(0==strcmp(datas->servicetype,
				"urn:schemas-upnp-org:service:WANPPPConnection:1") )
			datas->state = 4; */
	}
}

/* Data handler :
 * copy data depending on the current element name and state */
void IGDdata(void * d, const char * data, int l)
{
	struct IGDdatas * datas = (struct IGDdatas *)d;
	char * dstmember = 0;
	/*printf("%2d %s : %.*s\n",
           datas->level, datas->cureltname, l, data);	*/
	if( !XP_API_STRCMP(datas->cureltname, "URLBase") )
		dstmember = datas->urlbase;
	else if(datas->state<=1)
	{
		if( !XP_API_STRCMP(datas->cureltname, "serviceType") )
			dstmember = datas->servicetype_CIF;
		else if( !XP_API_STRCMP(datas->cureltname, "controlURL") )
			dstmember = datas->controlurl_CIF;
		else if( !XP_API_STRCMP(datas->cureltname, "eventSubURL") )
			dstmember = datas->eventsuburl_CIF;
		else if( !XP_API_STRCMP(datas->cureltname, "SCPDURL") )
			dstmember = datas->scpdurl_CIF;
		else if( !XP_API_STRCMP(datas->cureltname, "deviceType") )
			dstmember = datas->devicetype_CIF;
	}
	else if(datas->state==2)
	{
		if( !XP_API_STRCMP(datas->cureltname, "serviceType") )
			dstmember = datas->servicetype;
		else if( !XP_API_STRCMP(datas->cureltname, "controlURL") )
			dstmember = datas->controlurl;
		else if( !XP_API_STRCMP(datas->cureltname, "eventSubURL") )
			dstmember = datas->eventsuburl;
		else if( !XP_API_STRCMP(datas->cureltname, "SCPDURL") )
			dstmember = datas->scpdurl;
		else if( !XP_API_STRCMP(datas->cureltname, "deviceType") )
			dstmember = datas->devicetype;
	}
	if(dstmember)
	{
		if(l>=MINIUPNPC_URL_MAXSIZE)
			l = MINIUPNPC_URL_MAXSIZE-1;
		XP_API_MEMCPY(dstmember, data, l);
		dstmember[l] = '\0';
	}
}

void printIGD(struct IGDdatas * d)
{
	XP_DEBUG_OUT("urlbase = %s\n", d->urlbase);
	XP_DEBUG_OUT("WAN Device (Common interface config) :\n");
	XP_DEBUG_OUT(" deviceType = %s\n", d->devicetype_CIF);
	XP_DEBUG_OUT(" serviceType = %s\n", d->servicetype_CIF);
	XP_DEBUG_OUT(" controlURL = %s\n", d->controlurl_CIF);
	XP_DEBUG_OUT(" eventSubURL = %s\n", d->eventsuburl_CIF);
	XP_DEBUG_OUT(" SCPDURL = %s\n", d->scpdurl_CIF);
	XP_DEBUG_OUT("WAN Connection Device :\n");
	XP_DEBUG_OUT(" deviceType = %s\n", d->devicetype);
	XP_DEBUG_OUT(" servicetype = %s\n", d->servicetype);
	XP_DEBUG_OUT(" controlURL = %s\n", d->controlurl);
	XP_DEBUG_OUT(" eventSubURL = %s\n", d->eventsuburl);
	XP_DEBUG_OUT(" SCPDURL = %s\n", d->scpdurl);
}