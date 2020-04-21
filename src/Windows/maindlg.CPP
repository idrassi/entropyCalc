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

#include "StdAfx.h"
#include "resource.h"
#include "MainDlg.h"
#include "entropy.h"

LRESULT CMainDlg::OnColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HDC hdc = (HDC)	wParam;
	HWND hw = (HWND) lParam;

	if (hw == GetDlgItem (IDC_ENTROPY))
	{
		::SetTextColor(hdc, ::GetSysColor(COLOR_WINDOWTEXT));
		::SelectObject(hdc, m_brshBackGround);
		return (LRESULT) m_brshBackGround;
	}
	else
		return NULL;
}

LRESULT CMainDlg::OnBnClickedBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg (TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"All Files (*.*)\0*.*", m_hWnd);
	if (dlg.DoModal () == IDOK)
	{
		SetDlgItemText (IDC_PATH, dlg.m_szFileName);
	}

	return 0;
}


LRESULT CMainDlg::OnBnClickedCompute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	WCHAR szFilePath[2 * MAX_PATH];

	GetDlgItemText (IDC_PATH, szFilePath, ARRAYSIZE (szFilePath));
	SetDlgItemText (IDC_ENTROPY, L"");

	if (lstrlen (szFilePath) == 0)
	{
		Warning (L"No file path specified");
		return 0;
	}
	else if (_waccess (szFilePath, 0) != 0)
	{
		Warning (L"The specified file doesn't exist");
		return 0;
	}

	FILE* f = _wfopen (szFilePath, L"rb");
	if (f)
	{
		long long flen = _filelengthi64 (_fileno (f));
		if (flen == 0)
			Warning (L"The selected file is empty!");
		else
		{
			CWaitCursor busy;
			WCHAR szEntropy[32];
			eEntropyMethod meth;

			switch (m_list.GetCurSel())
			{
			case 1: meth = ENT_METH_GZIP; break;
			case 2: meth = ENT_METH_LZMA; break;
			case 3: meth = ENT_METH_MIN_ALL; break;
			default: meth = ENT_METH_FREQ; break;
			}

			double e = ComputeShannonEntropy (f, meth);
			
			StringCbPrintfW (szEntropy, sizeof (szEntropy), L"%f", e);
			SetDlgItemText (IDC_ENTROPY, szEntropy);
		}

		fclose (f);
	}
	else
		Error (L"The selected file could not be opened for reading.");


	return 0;
}


LRESULT CMainDlg::OnNMClickSyslink1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	ShellExecute (m_hWnd, L"open", L"https://www.idrix.fr", NULL, NULL, SW_SHOW);
	return 0;
}
