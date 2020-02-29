#include "stdafx.h"
#include "mainWindow.h"
#include "resource.h"




HINSTANCE mainInst;  
HWND mainWindowHandle;
HWND curContactDialog;
HWND emailText;
HWND emailLookup;
HWND emailButton;

HWND nameText;
HWND nameLookup;
HWND nameButton;

HWND phoneText;
HWND phoneLookup;
HWND phoneButton;

HWND curContactsList;
HWND createContactButton;

CRMBackEnd backEnd;

/*
//screw unicode crap, this is now a multi byte project
wstring ConvertToWindowsString(string s)
{
	 wchar_t *wtext = new wchar_t[s.size()];
	 mbstowcs(wtext, s.c_str(), s.size());//includes null
	 wstring ret = wtext;
	 return ret;
}*/
//the cords was created with a resrouce editor, everything else was done by hand :(
void InitMainWindow(HWND hDlg)
{
	//HFONT hfont3 = CreateFont(-11, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0, ("Ms Shell Dlg"));

	//HWND hCtrl3_0 = CreateWindowEx(0, WC_LISTBOX, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT | LBS_SORT | LBS_NOTIFY, 218, 203, 473, 260, hwnd, (HMENU)0, hInst, 0);
	mainWindowHandle = hDlg;
	emailText = CreateWindow( TEXT("STATIC"), TEXT("Email Lookup"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 8, 16, 66, 15, hDlg, (HMENU)IDC_EMAIL_LOOKUP_STATIC, NULL, NULL);
	emailLookup = CreateWindow(TEXT("Edit"),TEXT (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 83, 16, 360, 23, hDlg, (HMENU)IDC_EMAIL_LOOKUP, NULL, NULL);
	emailButton = CreateWindow(TEXT("BUTTON"), TEXT("Go!"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, 465, 16, 90, 23, hDlg, (HMENU)IDC_EMAIL_LOOKUP_OK, NULL, NULL);

	nameText = CreateWindow(TEXT("STATIC"), TEXT("Name Lookup"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 8, 57, 69, 15, hDlg, (HMENU)IDC_NAME_LOOKUP_STATIC, NULL, NULL);
	nameLookup = CreateWindow(TEXT("Edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 83, 57, 360, 23, hDlg, (HMENU)IDC_NAME_LOOKUP, NULL, NULL);
	nameButton = CreateWindow(TEXT("BUTTON"), TEXT("Go!"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, 465, 57, 90, 23, hDlg, (HMENU)IDC_NAME_LOOKUP_OK, NULL, NULL);


	phoneText = CreateWindow(TEXT("STATIC"), TEXT("Phone Lookup"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 8, 98, 72, 15, hDlg, (HMENU)IDC_PHONE_LOOKUP_STATIC, NULL, NULL);
	phoneLookup = CreateWindow(TEXT("Edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 83, 98, 360, 23, hDlg, (HMENU)IDC_PHONE_LOOKUP,NULL, NULL);
	phoneButton = CreateWindow(TEXT("BUTTON"), TEXT("Go!"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, 465, 98, 90, 23, hDlg, (HMENU)IDC_PHONE_LOOKUP_OK, NULL, NULL);

	
	curContactsList = CreateWindow(TEXT("STATIC"), TEXT("People To Contact Today"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 398, 179, 143, 16, hDlg, (HMENU)0, NULL, NULL);

	
	createContactButton = CreateWindow(TEXT("BUTTON"), TEXT("Create New Contact"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, 345, 512, 225, 98, hDlg, (HMENU)IDC_CREATE_CONTACT_OK, NULL, NULL);
}

//the cords was created with a resrouce editor, everything else was done by hand :(
void InitCurContactDialog(HWND hDlg,string searchItem)
{
	
	backEnd.DoEmailLookup(searchItem);
	curContactDialog = CreateDialog(mainInst, MAKEINTRESOURCE(IDD_CONTACT_VIEW), mainWindowHandle, CurContactView);

	//int x = GetDlgItemText(curContactDialog, IDC_EMAIL_LOOKUP, buffer, MAX_PATH);

	SetDlgItemText(curContactDialog, IDC_CONTACT_NAME,backEnd.curContact.name.c_str());
	SetDlgItemText(curContactDialog, IDC_CONTACT_EMAIL,backEnd.curContact.email.c_str());
	SetDlgItemText(curContactDialog, IDC_CONTACT_PHONE,backEnd.curContact.phone.c_str());
	SetDlgItemText(curContactDialog, IDC_CONTACT_NEXT_CONTACT_DATE,backEnd.curContact.nextContactDate.ToString().c_str());
	
	string uniqueName = "Unique Name: " + backEnd.curContact.email;
	SetDlgItemText(curContactDialog, IDC_CONTACT_UNIQUE_NAME,  uniqueName.c_str());
	
	//this does the same thing
	//HWND temp = GetDlgItem(curContactDialog, IDC_CONTACT_UNIQUE_NAME);
	//SetWindowText(temp, L"Unique Name: victor.reynolds");

	string numContacts = "Num Contacts: " + to_string(backEnd.curContact.numContacts);
	SetDlgItemText(curContactDialog, IDC_CONTACT_NUM_CONTACTS, numContacts.c_str());


	string lastContact = "Last Contact:" + backEnd.curContact.lastContactDate.ToString();
	SetDlgItemText(curContactDialog, IDC_CONTACT_LAST_CONTACT, lastContact.c_str());

	string lastContactType = "Last Contact Type: has not been implemented yet";//concat strings here
	SetDlgItemText(curContactDialog, IDC_CONTACT_LAST_CONTACT_TYPE, lastContactType.c_str());


	SetDlgItemText(curContactDialog, IDC_CONTACT_NOTES,backEnd.curContact.notes.c_str() );

    string contactAddedDate = "Contact Added: "+ backEnd.curContact.dateAdded.ToString();
	SetDlgItemText(curContactDialog, IDC_CONTACT_ADDED_DATE, contactAddedDate.c_str());


	ShowWindow(curContactDialog, SW_SHOW);
}

// Message handler for main contact box.
INT_PTR CALLBACK CurContactView(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            //update db info
			backEnd.SaveCurrentInfo();

			EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
		}
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL CheckInput(WPARAM wParam, std::string &output)
{
	if (wParam == IDC_EMAIL_LOOKUP_OK)
	{
		char buffer[MAX_PATH];
		int x = GetDlgItemText(mainWindowHandle, IDC_EMAIL_LOOKUP, buffer, MAX_PATH);
		if (x == 0)
		{
			x = GetLastError();
			char temp[12];

			//wsprintf(temp,L"error: %d", x);
			sprintf_s(temp,"error: %d", x);
			MessageBox(NULL, temp , NULL, NULL);
		}
		
		output = buffer;
		return (INT_PTR)TRUE;
	}
	

	return FALSE;
}