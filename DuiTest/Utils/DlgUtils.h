#ifndef _DLGUTILS_H_
#define _DLGUTILS_H_

namespace DlgUtils
{
	//�ж��ļ������Ƿ����
	bool InvalidNameString(CString & strName);

	bool DlgGetOpenFileName(
		CString& strFile, //�ļ��ľ���·��
		CString& strFileNameDotExt, //�ļ��� ��(name.txt)
		LPCTSTR strFilter, //������ ��(PNG\0*.png\0\0)
		HWND hWnd = NULL, //������
		CString strTitle = CString(), //�Զ��������ʾ Ĭ��Ϊ(���Ϊ���)
		CString strDefExt = CString(), //Ĭ�Ϻ�׺ ��(.txt)
		CString initPath = CString()); //��ʼ��Ŀ¼


	bool DlgGetSaveFileName(
		CString& strFile, //�ļ��ľ���·��
		CString& strFileNameDotExt, //�ļ��� ��(name.txt)
		LPCTSTR strFilter, //������ ��(PNG\0*.png\0\0)
		HWND hWnd = NULL, //������
		CString strTitle = CString(), //�Զ��������ʾ Ĭ��Ϊ(���Ϊ���)
		CString strDefExt = CString(), //Ĭ�Ϻ�׺ ��(.txt)
		CString initPath = CString()); //��ʼ��Ŀ¼

		bool DlgBrowseForFolder(
		CString& strPath, //Ŀ¼�ľ���·��
		CString& strEditCtx, //�༭�������,���UseEditʹ��
		CString strTitle, //����˵��
		HWND hWnd = NULL,
		//ʹ�ñ༭�������,����֤�ļ�����ȷ��.
		//ʧ��ʱ,ʹ��ѡ����ļ�����
		//�������ͨ��ʹ��"New"����
		BOOL UseEdit = FALSE);

	//�����ļ���ָ��Ŀ¼,�ļ������򵯿����û�ѡ�����
	bool DlgCopyReplace(CString strFileSrc, CString strFileDst);

}
#endif //_DLGUTILS_H_
