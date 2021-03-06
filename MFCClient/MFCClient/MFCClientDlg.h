
// MFCClientDlg.h: 头文件
//

#pragma once
#include "afxwin.h"
#include <winsock2.h>

// CMFCClientDlg 对话框
class CMFCClientDlg : public CDialog
{
// 构造
public:
	CMFCClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	SOCKET clientSocket;
	SOCKET udpclientSocket;
	sockaddr_in m_serverAddr;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 记录需要发送的Message
	CString messageString;
	afx_msg void OnEnChangeMessage();
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnEnChangePort();
	afx_msg void OnEnChangeIpadress();
	afx_msg void OnLbnSelchangeMessageslist();
	long OnSocket(WPARAM wParam, LPARAM lParam);
	BOOL Connect(LPCTSTR pszRemoteAddr, u_short nPort);
	afx_msg
		void AddStringToList(LPCTSTR pszText, BOOL bRecv);
	void OnBnClickedClear();
	// 讲述提示信息
	CString clueString;
	// 记录ip地址
	CString ipAddressString;
	// 记录端口
	CString portString;
	afx_msg void OnBnClickedSelectfile();
	CString BootOpenDialog();
	afx_msg void OnBnClickedUpload();
	// udpPortString
	CString udpPortString;
	CString udpMessageString;
	afx_msg void OnBnClickedUdpsend();
	void AddUDPStringToList(LPCTSTR pszText, BOOL bRecv);
};
