#pragma once
#include <string>
#include "Windows.h"
#include "CRMBackEnd.h"

#define MAX_LOADSTRING 100
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800


extern  HINSTANCE mainInst;  
extern  HWND mainWindowHandle;
extern  HWND curCustomerDialog;
extern  HWND addCustomerContactDialog;
extern 	HWND emailText;
extern 	HWND emailLookup;
extern 	HWND emailButton;

extern 	HWND nameText;
extern 	HWND nameLookup;
extern 	HWND nameButton;

extern 	HWND phoneText;
extern 	HWND phoneLookup;
extern 	HWND phoneButton;

extern 	HWND curContactsList;
extern 	HWND createContactButton;
extern  HWND mainListView;
extern  HWND succesfulContactCheck;
extern  HWND convoListView;
extern  HWND activeCustomerCheck;
extern CRMBackEnd backEnd;

extern vector<CRM::Customer> todaysContactList;
extern vector<CRM::ContactDetails> curCustomerConversationList;
void InitMainWindow(HWND hDlg);
bool ProcessMainScreenButtons(string searchItem, int searchType);
void InitCurCustomerDialog();

void FillCustomerStructFromCustomerDialog();
BOOL CheckMainWindowInput(WPARAM wParam, std::string &output);
INT_PTR CALLBACK CurCustomerView(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AddCustomerContactView(HWND, UINT, WPARAM, LPARAM);

HWND CreateListView(HWND hwndParent, int windowsID, vector<string> colNames, int x, int y, int w, int h, int size);
BOOL FillCustomerListViewItems(HWND hWndListView, vector<CRM::Customer> &items);
BOOL FillCustomerConvoListViewItems(HWND hWndListView, vector<CRM::ContactDetails> &items);
BOOL FillCustomerTagListViewItems(HWND hWndListView, vector<string> &items);

LRESULT MainWindowNotifyHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CusCustomerNotifyHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


/*
//screw unicode crap, this is now a multi byte project
wstring ConvertToWindowsString(string s)
{
	 wchar_t *wtext = new wchar_t[s.size()];
	 mbstowcs(wtext, s.c_str(), s.size());//includes null
	 wstring ret = wtext;
	 return ret;
}*/
