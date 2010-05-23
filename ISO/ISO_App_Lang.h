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

const WCHAR*const g_ISO_App_String[] = {
	L"更新补丁文件头部失败",//0
	L"GetHead error",
	L"写补丁文件头部失败",
	L"两ISO是不同的游戏",
	L"ISO类型不同",
	L"ISO还没打开呢",//5
	L"原ISO还没打开呢",
	L"写补丁失败\r\n",
	L"初始化临时文件错误",
	L"读取补丁文件错误",
	L"文件(%hs/%hs)的crc32校验失败\r\n当前:%08X\r\n原来:%08X",//10
	L"读取根目录失败",
	L"补丁文件参数错误(m_nFileCount)",
	L"补丁文件校验错误(m_uCrc32)",
	L"补丁文件参数错误(m_uSize)",
	L"此补丁不适用于本ISO",//15
	L"文件大小错误（nFileSize < head.m_nSize）",
	L"文件头校验错误（m_nSize）",
	L"文件头校验错误（m_uCrc32）",
	L"版本错误或者是不支持的版本",
	L"不是\"%hs\"文件",//20
	L"读补丁信息失败",
	L"%hs\r\n在ISO中找不到文件",
	L"%ls\r\n打开文件失败",
	L"%hs\r\n错误的ISO路径",
	L"%hs\r\n不能写ISO目录",//25
	L"%ls\r\n创建目录失败",
	L"%ls\r\n找不到输入文件",
	L"%ls\r\n取文件名失败",
	L"%ls\r\n文件大小错误",
	L"%ls\r\n写文件到ISO出错",//30
	L"%ls\r\n导出文件失败",
	L"%hs\r\n文件名只能用 字母 数字 点 下划线",
	L"%hs\r\n创建ISO目录失败",
	L"%hs\r\n目录名只能用 字母 数字 点 下划线",
	L"%ls\r\n打开ISO失败",//35
};
