#pragma once
#include <string>
#include "Windows.h"
#include "CRMBackEnd.h"

extern  HINSTANCE mainInst;  
extern  HWND mainWindowHandle;
extern  HWND curContactDialog;
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

extern CRMBackEnd backEnd;



void InitMainWindow(HWND hDlg);
void InitCurCustomerDialog(HWND hDlg, std::string searchItem,int searchType); 
void FillCustomerStructFromCustomerDialog();
BOOL CheckMainWindowInput(WPARAM wParam, std::string &output);
INT_PTR CALLBACK   CurCustomerView(HWND, UINT, WPARAM, LPARAM);
