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

string GetTextFieldData(HWND windowHandle, int dialogID,  bool msgboxOnError = true)
{
	char buffer[MAX_PATH];
	string ret;
	int x = GetDlgItemText(windowHandle, dialogID, buffer, MAX_PATH);
	if (x == 0 && msgboxOnError)
	{
		x = GetLastError();
		char temp[12];

		//wsprintf(temp,L"error: %d", x);
		sprintf_s(temp,"error: %d", x);
		MessageBox(NULL, temp , NULL, NULL);
	}
	ret = buffer;
	return ret;
}
//----------------------------------------------------------------------------------------------------------------
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

	
	curContactsList = CreateWindow(TEXT("STATIC"), TEXT("People To Customer Today"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 398, 179, 143, 16, hDlg, (HMENU)0, NULL, NULL);

	
	createContactButton = CreateWindow(TEXT("BUTTON"), TEXT("Create New Customer"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, 345, 512, 225, 98, hDlg, (HMENU)IDC_ADD_CUSTOMER, NULL, NULL);
}
//----------------------------------------------------------------------------------------------------------------
//the cords was created with a resrouce editor, everything else was done by hand :(
void InitCurCustomerDialog(HWND hDlg,string searchItem,int searchType)
{
	bool search = true;
	if (searchType == IDC_EMAIL_LOOKUP_OK)
		backEnd.DoEmailLookup(searchItem);
	else if(searchType == IDC_PHONE_LOOKUP_OK)
		backEnd.DoPhoneLookup(searchItem);
	else if(searchType == IDC_NAME_LOOKUP_OK)
		backEnd.DoNameLookup(searchItem);

	if (searchType == IDC_ADD_CUSTOMER)
	{
		search = false;
		backEnd.curContact.Clear();
		backEnd.curContact.dateAdded = DateTime::Now();
		backEnd.curContact.lastContactDate = DateTime::Now();
	}

	if(search && backEnd.curContact.id < 1)
	{
		MessageBox(NULL, "Contact is not in the database" , NULL, NULL);
		return;
	}

	
	curContactDialog = CreateDialog(mainInst, MAKEINTRESOURCE(IDD_CONTACT_VIEW), mainWindowHandle, CurCustomerView);

	
	//int x = GetDlgItemText(curContactDialog, IDC_EMAIL_LOOKUP, buffer, MAX_PATH);

	SetDlgItemText(curContactDialog, IDC_CUSTOMER_NAME,backEnd.curContact.name.c_str());
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_EMAIL,backEnd.curContact.email.c_str());
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_PHONE,backEnd.curContact.phone.c_str());
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_NEXT_CONTACT_DATE,backEnd.curContact.nextContactDate.ToString().c_str());
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_UNIQUE_NAME,  backEnd.curContact.uniqueName.c_str());
	
	//this does the same thing
	//HWND temp = GetDlgItem(curContactDialog, IDC_CUSTOMER_UNIQUE_NAME);
	//SetWindowText(temp, L"Unique Name: victor.reynolds");

	string numContacts = "Num Contacts: " + to_string(backEnd.curContact.numContacts);
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_NUM_CONTACTS, numContacts.c_str());


	string lastContact = "Last Contact Date:" + backEnd.curContact.lastContactDate.ToString();
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_LAST_CONTACT, lastContact.c_str());

	string lastContactType = "Last Contact Type: has not been implemented yet";//concat strings here
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_LAST_CONTACT_TYPE, lastContactType.c_str());


	SetDlgItemText(curContactDialog, IDC_CUSTOMER_NOTES,backEnd.curContact.notes.c_str() );

    string contactAddedDate = "Customer Added: "+ backEnd.curContact.dateAdded.ToString();
	SetDlgItemText(curContactDialog, IDC_CUSTOMER_ADDED_DATE, contactAddedDate.c_str());


	ShowWindow(curContactDialog, SW_SHOW);
}
//----------------------------------------------------------------------------------------------------------------
//the cords was created with a resrouce editor, everything else was done by hand :(
void FillCustomerStructFromCustomerDialog()
{
	backEnd.curContact.name = GetTextFieldData(curContactDialog, IDC_CUSTOMER_NAME);
	backEnd.curContact.email = GetTextFieldData(curContactDialog, IDC_CUSTOMER_EMAIL);
	backEnd.curContact.phone = GetTextFieldData(curContactDialog, IDC_CUSTOMER_PHONE);
	backEnd.curContact.uniqueName = GetTextFieldData(curContactDialog, IDC_CUSTOMER_UNIQUE_NAME);
	
	backEnd.curContact.nextContactDate = DateTime (GetTextFieldData(curContactDialog, IDC_CUSTOMER_NEXT_CONTACT_DATE));
	backEnd.curContact.lastContactDate = DateTime::Now();
	backEnd.curContact.dateAdded = DateTime::Now();
	backEnd.curContact.notes = GetTextFieldData(curContactDialog, IDC_CUSTOMER_NOTES);

	/*if (!backEnd.curContact.email.empty())
		backEnd.curContact.uniqueName = backEnd.curContact.email;
	else
		backEnd.curContact.uniqueName = backEnd.curContact.name + "_"+DateTime::Now();*/
}
//----------------------------------------------------------------------------------------------------------------
// Message handler for main customer box.
INT_PTR CALLBACK CurCustomerView(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_SAVE_CUSTOMER)
        {
            //update db info
			FillCustomerStructFromCustomerDialog();
			if (!backEnd.SaveCurrentInfo())
			{
				MessageBox(NULL, "error saving to database...try again?", NULL, NULL);
			}
			else
			{
				backEnd.curContact.Clear();
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
        }
		if (LOWORD(wParam) == IDC_ADD_CONTACT)
        {
          
			MessageBox(NULL, "this will be wheer to add a contact detail", NULL, NULL);
		
        }

		if (LOWORD(wParam) == IDCANCEL)
		{
			backEnd.curContact.Clear();
			EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
		}
        break;
    }
    return (INT_PTR)FALSE;
}
//----------------------------------------------------------------------------------------------------------------
BOOL CheckMainWindowInput(WPARAM wParam, std::string &output)
{
	
	if (wParam == IDC_EMAIL_LOOKUP_OK)
	{
		output = GetTextFieldData(mainWindowHandle, IDC_EMAIL_LOOKUP);
		return (INT_PTR)TRUE;
	}
	if (wParam == IDC_NAME_LOOKUP_OK)
	{
		output = GetTextFieldData(mainWindowHandle, IDC_NAME_LOOKUP);
		return (INT_PTR)TRUE;
	}
	if (wParam == IDC_PHONE_LOOKUP_OK)
	{
		output = GetTextFieldData(mainWindowHandle, IDC_PHONE_LOOKUP);
		return (INT_PTR)TRUE;
	}
	else if (wParam == IDC_ADD_CUSTOMER)
	{
		return (INT_PTR)TRUE;
	}
	return FALSE;
}