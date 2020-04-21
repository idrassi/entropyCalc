/** 

EntropyCal - A tool to compute an estimation of a file entropy

Copyright (c) 2020 Mounir IDRASSI (mounir@idrix.fr)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

**/

#pragma once

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };
	CComboBox m_list;
	HBRUSH m_brshBackGround;

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_BROWSE, BN_CLICKED, OnBnClickedBrowse)
		COMMAND_HANDLER(IDC_COMPUTE, BN_CLICKED, OnBnClickedCompute)
		NOTIFY_HANDLER(IDC_SYSLINK1, NM_CLICK, OnNMClickSyslink1)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorDlg)
	END_MSG_MAP()

	CMainDlg ()
	{
		m_brshBackGround = ::CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
	}

	~CMainDlg()
	{
		::DeleteObject(m_brshBackGround);
	}

	void Warning (LPCWSTR szMsg)
	{
		MessageBox (szMsg, L"Warning", MB_ICONWARNING);
	}

	void Error (LPCWSTR szMsg)
	{
		MessageBox (szMsg, L"Error", MB_ICONERROR);
	}

	void Info (LPCWSTR szMsg)
	{
		MessageBox (szMsg, L"Information", MB_ICONINFORMATION);
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		m_list.Attach (GetDlgItem (IDC_LIST));
		m_list.AddString (L"Bytes Frequency");
		m_list.AddString (L"GZip Compression");
		m_list.AddString(L"LZMA Compression");
		m_list.AddString(L"Minimum of all methods");
		m_list.SetCurSel (0);

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		return TRUE;
	}


	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
	LRESULT OnBnClickedBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCompute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNMClickSyslink1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
};
