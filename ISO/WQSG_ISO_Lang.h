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
#pragma once

const WCHAR*const g_WQSG_ISO_String[] = {
	L"???",//0
	L"未知的 2352 Mode",
	L"创建LBA分配表失败",
	L"设置LBA保留区失败",
	L"分配 根目录LBA失败",
	L"不是 CD001",//5
	L"不支持的 卷描述 %d.%d",
	L"逻辑块大小数不同",
	L"逻辑块Size不为 2048 , (%d)",
	L"根目录 size 错误",
	L"LBA大小总数不同",//10
	L"LBAPathTableSize大小不同",
	L"v6大小不同",
	L"根目录校验失败",
	L"不支持的子ISO系统(ISO9660)",
	L"不支持的子ISO系统(JOLIET)",//15
	L"不支持的子ISO系统(未知)",
	L"参数错误",
	L"参数错误 nLbaIndex(%d) >= nLbaCount(%d)",
	L"参数错误 a_ParentDirEnt.size_le = %08X , nLbaOffset = %d , (2048-DEF_FN_make_DirLen(0)) = %d",
	L"读数据出错",//20
	L"错误的目录项,文件名长度错误",
	L"错误的目录项,目录项校验错误",
	L"参数错误,错误的DirEnt",
	L"不是DirEnt(1)",
	L"不是DirEnt(2)",//25
	L"申请LBA失败",
	L"不能查找空文件名",
	L"没找到文件",
	L"dir预定长度超过250",
	L"dir实际长度 != 预定长度",//30
	L"文件长度不足",
	L"读取数据错误",
	L"输出数据错误",
	L"预定长度超过250",
	L"不能写目录",//35
	L"创建目录失败,已存在同名文件",
	L"目录项长度 与 预定长度不符",
	L"释放LBA失败",
	L"ISO空间不足,尝试扩容ISO失败",
	L"释放LBA失败,这不可能",//40
	L"申请LBA失败,这不可能,怎么会这样",
	L"当前LAB块不足以容得下写入的数据，你可以先写偏移0，扩大文件后再写偏移",
	L"添加文件只能从偏移 0 开始",
	L"目录空间不足",
	L"写文件失败",//45
	L"读文件失败",
	L"文件结构版本不为 1",
	L"此ISO不是UMD",
	L"文件结构版本不为 2 or 1",
	L"读取ISO信息失败",//50
};
