/*  WQSG_Lib
*  Copyright (C) WQSG <wqsg@qq.com>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
#ifndef __WQSG_UMD_H__
#define __WQSG_UMD_H__

#include "wqsg_iso_base.h"

class CWQSG_UMD : public CWQSG_ISO_Interface
{
public:
	//------------------------------------------
private:
protected:
	//----------------------------------------------
public:
	//------------------------------------------
	CWQSG_UMD(void);
	virtual ~CWQSG_UMD(void);
	//------------------------------------------
	virtual	BOOL OpenISO( const WCHAR*const a_isoPathName , const BOOL a_bCanWrite );
	virtual	void CloseISO();

	BOOL GetUmdID( u8 a_IdBuffer[32] );

	virtual BOOL IsOpen()
	{
		return CWQSG_ISO_Interface::IsOpen();
	}
	//------------------------------------------
};

#endif //__WQSG_UMD_H__
