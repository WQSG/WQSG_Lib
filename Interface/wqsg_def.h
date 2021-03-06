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
#ifndef __Interface_WQSG_DEF_H__
#define __Interface_WQSG_DEF_H__

#include "../wqsg_def.h"

#define WQSG_BIT(x) (1<<(x))

#define		__i__ 


template<typename T>
class CSingletonT
{
protected:
	CSingletonT(){}
	virtual ~CSingletonT(){}
public:
	static T& I()
	{
		static T _val;
		return _val;
	}
};


#endif //__Interface_WQSG_DEF_H__
