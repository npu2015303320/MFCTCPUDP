
// MFCClientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MFCClient.h"
#include "MFCClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// 忽略4996警告
#pragma warning(disable : 4996)

#pragma warning(disable : 4086)
// 定义网络事件通知消息
#define WM_SOCKET WM_USER + 1	
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCClientDlg 对话框



CMFCClientDlg::CMFCClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MFCCLIENT_DIALOG, pParent)
	, messageString(_T(""))
	, clueString(_T(""))
	, ipAddressString(_T(""))
	, portString(_T(""))
	, udpPortString(_T(""))
	, udpMessageString(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MESSAGE, messageString);
	DDX_Text(pDX, IDC_CLUE, clueString);
	DDX_Text(pDX, IDC_IPADRESS, ipAddressString);
	DDV_MaxChars(pDX, ipAddressString, 15);
	DDX_Text(pDX, IDC_PORT, portString);
	DDV_MaxChars(pDX, portString, 6);
	DDX_Text(pDX, IDC_UDPPORT, udpPortString);
	DDX_Text(pDX, IDC_UDPMESSAGE, udpMessageString);
	DDV_MaxChars(pDX, udpPortString, 6);
}

BEGIN_MESSAGE_MAP(CMFCClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_MESSAGE, &CMFCClientDlg::OnEnChangeMessage)
	ON_BN_CLICKED(IDC_SEND, &CMFCClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_EXIT, &CMFCClientDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_CONNECT, &CMFCClientDlg::OnBnClickedConnect)
	ON_EN_CHANGE(IDC_PORT, &CMFCClientDlg::OnEnChangePort)
	ON_EN_CHANGE(IDC_IPADRESS, &CMFCClientDlg::OnEnChangeIpadress)
	ON_BN_CLICKED(IDC_CLEAR, &CMFCClientDlg::OnBnClickedClear)
	ON_MESSAGE(WM_SOCKET, OnSocket)
	ON_BN_CLICKED(IDC_SELECTFILE, &CMFCClientDlg::OnBnClickedSelectfile)
	ON_BN_CLICKED(IDC_UPLOAD, &CMFCClientDlg::OnBnClickedUpload)
	ON_BN_CLICKED(IDC_UDPSEND, &CMFCClientDlg::OnBnClickedUdpsend)
END_MESSAGE_MAP()


// CMFCClientDlg 消息处理程序

BOOL CMFCClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 初始化Winsock库
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);
	// 记录提示消息
	clueString = "空闲";
	UpdateData(FALSE);

	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_MESSAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UDPMESSAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UDPSEND)->EnableWindow(FALSE);
	// 初始化连接套接字
	clientSocket = INVALID_SOCKET;
	udpclientSocket = INVALID_SOCKET;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCClientDlg::OnEnChangeMessage()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CMFCClientDlg::OnBnClickedSend()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
	if (clientSocket == INVALID_SOCKET)
	{
		return;
	}

	// 取得要发送的字符串
	CString sText;
	sText = messageString;
	// 添加一个“回车换行”
	// 注意，添加它并不是必须的，但是如果使用本软件作为客户端调试网络协议，
	// 比如SMTP、FTP等，就要添加它了。因为这些协议都要求使用“回车换行”作为一个命令的结束标记
	sText += "\r\n";

	// 发送数据到服务器
	if (::send(clientSocket, sText, sText.GetLength(), 0) != -1)
	{
		AddStringToList(sText, FALSE);
		messageString = "";
		UpdateData(FALSE);
	}
}


void CMFCClientDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	if (clientSocket != INVALID_SOCKET)
		::closesocket(clientSocket);

	CDialog::OnCancel();
}


void CMFCClientDlg::OnBnClickedConnect()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
	if (clientSocket == INVALID_SOCKET)  // 连接服务器
	{
		// 取得服务器地址		
		if (ipAddressString.IsEmpty())
		{
			MessageBox("请输入服务器地址！");
			return;
		}

		// 取得端口号
		int nPort = atoi(portString);
		if (nPort < 1 || nPort > 65535)
		{
			MessageBox("端口号错误！");
			return;
		}
		int uPort = atoi(udpPortString);
		if (uPort < 1 || uPort > 65535)
		{
			MessageBox("端口号错误！");
			return;
		}
		// 试图连接服务器
		if (!Connect(ipAddressString, nPort))
		{
			MessageBox("连接服务器出错！");
			return;
		}

		udpclientSocket = socket(AF_INET, SOCK_DGRAM, 0);
		m_serverAddr.sin_family = AF_INET;
		//MessageBox(tempStr);  
		m_serverAddr.sin_port = htons(uPort);
		m_serverAddr.sin_addr.s_addr = inet_addr(ipAddressString);

		// 设置用户界面
		GetDlgItem(IDC_CONNECT)->SetWindowText("取消");
		clueString = "空闲";
		UpdateData(FALSE);
	}
	else				// 断开服务器
	{
		// 关闭套节字
		::closesocket(clientSocket);
		::closesocket(udpclientSocket);
		clientSocket = INVALID_SOCKET;
		udpclientSocket = INVALID_SOCKET;
		// 设置用户界面
		GetDlgItem(IDC_CONNECT)->SetWindowText("连接服务器");
		clueString = "空闲";
		UpdateData(FALSE);
		GetDlgItem(IDC_IPADRESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_UDPPORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_MESSAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_UDPMESSAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_UDPSEND)->EnableWindow(FALSE);
	}
}


void CMFCClientDlg::OnEnChangePort()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CMFCClientDlg::OnEnChangeIpadress()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CMFCClientDlg::OnLbnSelchangeMessageslist()
{
	// TODO: 在此添加控件通知处理程序代码
}

long CMFCClientDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	// 取得有事件发生的套节字句柄
	SOCKET s = wParam;
	// 查看是否出错
	if (WSAGETSELECTERROR(lParam))
	{
		if (clientSocket != SOCKET_ERROR)
			OnBnClickedConnect();
		clueString = "连接出错";
		UpdateData(FALSE);
		return 0;
	}
	// 处理发生的事件
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:	// 套节字正确的连接到服务器
	{
		// 设置用户界面
		GetDlgItem(IDC_CONNECT)->SetWindowText("断开连接");

		GetDlgItem(IDC_CLUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADRESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_UDPPORT)->EnableWindow(FALSE);

		GetDlgItem(IDC_MESSAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SEND)->EnableWindow(TRUE);

		GetDlgItem(IDC_UDPMESSAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_UDPSEND)->EnableWindow(TRUE);
		clueString = "已经连接到服务器";
		UpdateData(FALSE);
	}
	break;

	case FD_READ:		// 套接字接受到对方发送过来的数据包
	{
		// 从服务器接受数据
		char szText[1024] = { 0 };
		::recv(s, szText, 1024, 0);
		// 显示给用户
		AddStringToList(CString(szText) + "\r\n", TRUE);
	}
	break;

	case FD_CLOSE:
		OnBnClickedConnect();
		break;
	}

	return 0;
}

BOOL CMFCClientDlg::Connect(LPCTSTR pszRemoteAddr, u_short nPort)
{
	// 创建套节字
	clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		return FALSE;
	}

	// 设置socket为窗口通知消息类型
	::WSAAsyncSelect(clientSocket, m_hWnd,
		WM_SOCKET, FD_CONNECT | FD_CLOSE | FD_WRITE | FD_READ);

	// 假定szAddr是IP地址
	ULONG uAddr = ::inet_addr(pszRemoteAddr);
	if (uAddr == INADDR_NONE)
	{
		// 不是IP地址，就认为这是主机名称
		// 从主机名取得IP地址
		hostent* pHost = ::gethostbyname(pszRemoteAddr);
		if (pHost == NULL)
		{
			::closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			return FALSE;
		}
		// 得到以网络字节顺序排列的IP地址
		uAddr = ((struct in_addr*)*(pHost->h_addr_list))->s_addr;
	}

	// 填写服务器地址信息
	sockaddr_in remote;
	remote.sin_addr.S_un.S_addr = uAddr;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(nPort);

	// 连接到远程机
	::connect(clientSocket, (sockaddr*)&remote, sizeof(sockaddr));

	return TRUE;
}


void CMFCClientDlg::AddStringToList(LPCTSTR pszText, BOOL bRecv)
{
	CString strEdit;
	GetDlgItem(IDC_MESSAGES)->GetWindowText(strEdit);

	if (bRecv)
	{
		strEdit += "TCP【Recv】：";
		strEdit += pszText;
	}
	else
	{
		strEdit += "TCP【Send】：";
		strEdit += pszText;
	}
	GetDlgItem(IDC_MESSAGES)->SetWindowText(strEdit);
}

void CMFCClientDlg::OnBnClickedClear()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_MESSAGES)->SetWindowText("");
}

//重载OnOK函数  
void CMFCClientDlg::OnOK()
{
	//空  
}


//重写虚函数PreTranslateMessage() 用于回车键确认  
BOOL CMFCClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类  
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
		else
			return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	else
		return CDialog::PreTranslateMessage(pMsg);
}

void CMFCClientDlg::OnBnClickedSelectfile()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: Add your control notification handler code here  
	CString m_strDescPath = "";        //类的成员变量  
	//"打开文件"对话框，选择文件，返回其路径  
	m_strDescPath = BootOpenDialog();

}
CString CMFCClientDlg::BootOpenDialog()   //返回选择的文件名称  
{
	CString strFile = _T("");

	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
	}

	return strFile;
}



void CMFCClientDlg::OnBnClickedUpload()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCClientDlg::OnBnClickedUdpsend()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	sendto(udpclientSocket, udpMessageString, strlen(udpMessageString), 0, (struct sockaddr*)&m_serverAddr, sizeof(sockaddr));
	udpMessageString =  udpMessageString + "\r\n";
	AddUDPStringToList(udpMessageString, FALSE);
	udpMessageString = "";
	UpdateData(FALSE);
}
void CMFCClientDlg::AddUDPStringToList(LPCTSTR pszText, BOOL bRecv)
{
	CString strEdit;
	GetDlgItem(IDC_MESSAGES)->GetWindowText(strEdit);

	if (bRecv)
	{
		strEdit += "UDP【Recv】：";
		strEdit += pszText;
	}
	else
	{
		strEdit += "UDP【Send】：";
		strEdit += pszText;
	}
	GetDlgItem(IDC_MESSAGES)->SetWindowText(strEdit);
}