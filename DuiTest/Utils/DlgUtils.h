#ifndef _DLGUTILS_H_
#define _DLGUTILS_H_

namespace DlgUtils
{
	//判断文件名称是否符合
	bool InvalidNameString(CString & strName);

	bool DlgGetOpenFileName(
		CString& strFile, //文件的绝对路径
		CString& strFileNameDotExt, //文件名 如(name.txt)
		LPCTSTR strFilter, //过滤器 如(PNG\0*.png\0\0)
		HWND hWnd = NULL, //父窗口
		CString strTitle = CString(), //自定义标题显示 默认为(另存为或打开)
		CString strDefExt = CString(), //默认后缀 如(.txt)
		CString initPath = CString()); //初始化目录


	bool DlgGetSaveFileName(
		CString& strFile, //文件的绝对路径
		CString& strFileNameDotExt, //文件名 如(name.txt)
		LPCTSTR strFilter, //过滤器 如(PNG\0*.png\0\0)
		HWND hWnd = NULL, //父窗口
		CString strTitle = CString(), //自定义标题显示 默认为(另存为或打开)
		CString strDefExt = CString(), //默认后缀 如(.txt)
		CString initPath = CString()); //初始化目录

		bool DlgBrowseForFolder(
		CString& strPath, //目录的绝对路径
		CString& strEditCtx, //编辑框的内容,配合UseEdit使用
		CString strTitle, //标题说明
		HWND hWnd = NULL,
		//使用编辑框的内容,并验证文件名正确性.
		//失败时,使用选择的文件夹名
		//如果都不通过使用"New"代替
		BOOL UseEdit = FALSE);

	//拷贝文件到指定目录,文件存在则弹框让用户选择操作
	bool DlgCopyReplace(CString strFileSrc, CString strFileDst);

}
#endif //_DLGUTILS_H_
