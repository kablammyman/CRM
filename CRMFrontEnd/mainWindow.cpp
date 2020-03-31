#include "stdafx.h"
#include "mainWindow.h"
#include "resource.h"
#include <Commctrl.h> //for listview
#include <vector>



HINSTANCE mainInst;  
HWND mainWindowHandle;
HWND curCustomerDialog;
HWND addCustomerContactDialog;
HWND emailText;
HWND emailLookup;
HWND emailButton;

HWND nameText;
HWND nameLookup;
HWND nameButton;

HWND phoneText;
HWND phoneLookup;
HWND phoneButton;
HWND succesfulContactCheck;
HWND activeCustomerCheck;
HWND curContactsList;
HWND createContactButton;
HWND mainListView;
HWND convoListView;
CRMBackEnd backEnd;
vector<CRM::Customer> todaysContactList;
vector<CRM::ContactDetails> curCustomerConversationList;

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
string GetTextAreaData(HWND windowHandle, int dialogID,  bool msgboxOnError = true)
{
	char buffer[1000];
	string ret;
	int x = GetDlgItemText(windowHandle, dialogID, buffer, 1000);
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
void RefreshMainWindowsContactList()
{
	if (mainListView == nullptr)
		return;
	ListView_DeleteAllItems(mainListView);
	todaysContactList.clear();
	backEnd.FillTodaysCusterContactList(todaysContactList);
	FillCustomerListViewItems(mainListView, todaysContactList);
}
//----------------------------------------------------------------------------------------------------------------
//the cords was created with a resrouce editor, everything else was done by hand :(
void InitMainWindow(HWND hDlg)
{
	//HFONT hfont3 = CreateFont(-11, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0, ("Ms Shell Dlg"));
	int staticX = 8,editX = 83, buttonX = 465;
	int bigButtonWidth = 225;
	//HWND hCtrl3_0 = CreateWindowEx(0, WC_LISTBOX, (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT | LBS_SORT | LBS_NOTIFY, 218, 203, 473, 260, hwnd, (HMENU)0, hInst, 0);
	mainWindowHandle = hDlg;
	emailText = CreateWindow( TEXT("STATIC"), TEXT("Email Lookup"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, staticX, 16, 66, 15, hDlg, (HMENU)IDC_EMAIL_LOOKUP_STATIC, NULL, NULL);
	emailLookup = CreateWindow(TEXT("Edit"),TEXT (""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, editX, 16, 360, 23, hDlg, (HMENU)IDC_EMAIL_LOOKUP, NULL, NULL);
	emailButton = CreateWindow(TEXT("BUTTON"), TEXT("Go!"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, buttonX, 16, 90, 23, hDlg, (HMENU)IDC_EMAIL_LOOKUP_OK, NULL, NULL);

	nameText = CreateWindow(TEXT("STATIC"), TEXT("Name Lookup"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, staticX, 57, 69, 15, hDlg, (HMENU)IDC_NAME_LOOKUP_STATIC, NULL, NULL);
	nameLookup = CreateWindow(TEXT("Edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, editX, 57, 360, 23, hDlg, (HMENU)IDC_NAME_LOOKUP, NULL, NULL);
	nameButton = CreateWindow(TEXT("BUTTON"), TEXT("Go!"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, buttonX, 57, 90, 23, hDlg, (HMENU)IDC_NAME_LOOKUP_OK, NULL, NULL);


	phoneText = CreateWindow(TEXT("STATIC"), TEXT("Phone Lookup"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, staticX, 98, 72, 15, hDlg, (HMENU)IDC_PHONE_LOOKUP_STATIC, NULL, NULL);
	phoneLookup = CreateWindow(TEXT("Edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, editX, 98, 360, 23, hDlg, (HMENU)IDC_PHONE_LOOKUP,NULL, NULL);
	phoneButton = CreateWindow(TEXT("BUTTON"), TEXT("Go!"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, buttonX, 98, 90, 23, hDlg, (HMENU)IDC_PHONE_LOOKUP_OK, NULL, NULL);

	
	curContactsList = CreateWindow(TEXT("STATIC"), TEXT("People To Contact Today"), WS_VISIBLE | WS_CHILD | WS_GROUP | SS_LEFT, 398, 170, 143, 16, hDlg, (HMENU)0, NULL, NULL);
	vector<string> headers = { "name","email","next contact","last contact","days over due" };
	mainListView = CreateListView(mainWindowHandle, IDM_LIST_VIEW_RESULTS, headers,staticX, 190, WINDOW_WIDTH - (staticX*4)  ,WINDOW_HEIGHT/2,150);

	
	RefreshMainWindowsContactList();
	createContactButton = CreateWindow(TEXT("BUTTON"), TEXT("Create New Customer"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | 0x00000001, (WINDOW_WIDTH /2) - (bigButtonWidth/2), WINDOW_HEIGHT - 194, bigButtonWidth, 98, hDlg, (HMENU)IDC_ADD_CUSTOMER, NULL, NULL);
}
//----------------------------------------------------------------------------------------------------------------
bool ProcessMainScreenButtons(string searchItem,int searchType)
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
		backEnd.curCustomer.Clear();
		backEnd.curCustomer.dateAdded = DateTime::Now();
		backEnd.curCustomer.lastContactDate = DateTime::Now();
	}

	if(search && backEnd.curCustomer.id < 1)
	{
		MessageBox(NULL, "Contact is not in the database" , NULL, NULL);
		return false;
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------
//the cords was created with a resrouce editor, everything else was done by hand :(
void InitCurCustomerDialog()
{	
	curCustomerDialog = CreateDialog(mainInst, MAKEINTRESOURCE(IDD_CONTACT_VIEW), mainWindowHandle, CurCustomerView);
		
	//int x = GetDlgItemText(curContactDialog, IDC_EMAIL_LOOKUP, buffer, MAX_PATH);

	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_NAME,backEnd.curCustomer.name.c_str());
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_EMAIL,backEnd.curCustomer.email.c_str());
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_PHONE,backEnd.curCustomer.phone.c_str());
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_NEXT_CONTACT_DATE,backEnd.curCustomer.nextContactDate.ToString().c_str());
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_UNIQUE_NAME,  backEnd.curCustomer.uniqueName.c_str());
	
	//this does the same thing
	//HWND temp = GetDlgItem(curContactDialog, IDC_CUSTOMER_UNIQUE_NAME);
	//SetWindowText(temp, L"Unique Name: victor.reynolds");

	string numContacts = "Num Contacts: " + to_string(backEnd.curCustomer.numContacts);
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_NUM_CONTACTS, numContacts.c_str());


	string lastContact = "Last Contact Date:" + backEnd.curCustomer.lastContactDate.ToString();
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_LAST_CONTACT, lastContact.c_str());

	string lastContactType = "Last Contact Type: has not been implemented yet";//concat strings here
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_LAST_CONTACT_TYPE, lastContactType.c_str());


	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_NOTES,backEnd.curCustomer.notes.c_str() );

    string contactAddedDate = "Customer Added: "+ backEnd.curCustomer.dateAdded.ToString();
	SetDlgItemText(curCustomerDialog, IDC_CUSTOMER_ADDED_DATE, contactAddedDate.c_str());

	backEnd.FillCurCusterContactList(backEnd.curCustomer.id, curCustomerConversationList);

	vector<string> headers = { "Date","type","notes","Successful"};
	convoListView = CreateListView(curCustomerDialog,IDC_CONVO_LIST_VIEW, headers, 460, 300, 400, 185, 100);

	activeCustomerCheck = GetDlgItem(curCustomerDialog, IDC_IS_ACTIVE_CUSTOMER);
	if (backEnd.curCustomer.active)
		SendMessage(activeCustomerCheck, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(activeCustomerCheck, BM_SETCHECK, BST_UNCHECKED, 0);
	
	FillCustomerConvoListViewItems(convoListView, curCustomerConversationList);
	ShowWindow(curCustomerDialog, SW_SHOW);
}
//----------------------------------------------------------------------------------------------------------------
//the cords was created with a resrouce editor, everything else was done by hand :(
void InitAddCustomerContactDialog(bool isNewContact)
{	
	addCustomerContactDialog = CreateDialog(mainInst, MAKEINTRESOURCE(IDD_ADD_CUSTOMER_CONTACT_VIEW), mainWindowHandle, AddCustomerContactView);
	succesfulContactCheck = GetDlgItem(addCustomerContactDialog, IDC_IS_SUCCESSFUL_CONTACT);

	if (!isNewContact)
	{
		SetDlgItemText(addCustomerContactDialog, IDC_CONTACT_NOTES, backEnd.curContactDetails.notes.c_str());
		SetDlgItemText(addCustomerContactDialog, IDC_CONTACT_TYPE, backEnd.curContactDetails.type.c_str());
		SetDlgItemText(addCustomerContactDialog, IDC_DATE_CONTACTED, backEnd.curContactDetails.date.ToString().c_str());
		if(backEnd.curContactDetails.successfulContact)
			SendMessage(succesfulContactCheck, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage(succesfulContactCheck, BM_SETCHECK, BST_UNCHECKED, 0);

	}

	backEnd.curContactDetails.customerID = backEnd.curCustomer.id;
	ShowWindow(addCustomerContactDialog, SW_SHOW);
}
//----------------------------------------------------------------------------------------------------------------
//the cords was created with a resrouce editor, everything else was done by hand :(
void FillCustomerStructFromCustomerDialog()
{
	bool msgBoxOnError = false;

	//when adding new contact info, some forms could be blank, and thats fine
	//if (backEnd.curCustomer.id < 1)
	//	msgBoxOnError = true;

	backEnd.curCustomer.name = GetTextFieldData(curCustomerDialog, IDC_CUSTOMER_NAME,msgBoxOnError);
	backEnd.curCustomer.email = GetTextFieldData(curCustomerDialog, IDC_CUSTOMER_EMAIL,msgBoxOnError);
	backEnd.curCustomer.phone = GetTextFieldData(curCustomerDialog, IDC_CUSTOMER_PHONE,msgBoxOnError);
	backEnd.curCustomer.uniqueName = GetTextFieldData(curCustomerDialog, IDC_CUSTOMER_UNIQUE_NAME,msgBoxOnError);
	
	backEnd.curCustomer.nextContactDate = DateTime (GetTextFieldData(curCustomerDialog, IDC_CUSTOMER_NEXT_CONTACT_DATE));
	backEnd.curCustomer.lastContactDate = DateTime::Now();
	backEnd.curCustomer.dateAdded = DateTime::Now();
	//this has a largert buffer for notes
	backEnd.curCustomer.notes = GetTextAreaData(curCustomerDialog, IDC_CUSTOMER_NOTES,msgBoxOnError);

	/*if (!backEnd.curCustomer.email.empty())
		backEnd.curCustomer.uniqueName = backEnd.curCustomer.email;
	else
		backEnd.curCustomer.uniqueName = backEnd.curCustomer.name + "_"+DateTime::Now();*/
}
//----------------------------------------------------------------------------------------------------------------
//the cords was created with a resrouce editor, everything else was done by hand :(
void FillCustomerContactStructFromContactDialog()
{
	bool msgBoxOnError = false;

	backEnd.curContactDetails.type = GetTextFieldData(addCustomerContactDialog, IDC_CONTACT_TYPE,msgBoxOnError);
	backEnd.curContactDetails.date = DateTime::Now();// GetTextFieldData(curContactDialog, IDC_CUSTOMER_PHONE, msgBoxOnError);
	backEnd.curContactDetails.customerID = backEnd.curCustomer.id; // GetTextFieldData(curContactDialog, IDC_CUSTOMER_UNIQUE_NAME, msgBoxOnError);

	//this has a largert buffer for notes
	backEnd.curContactDetails.notes = GetTextAreaData(addCustomerContactDialog, IDC_CONTACT_NOTES,msgBoxOnError);
}

//----------------------------------------------------------------------------------------------------------------
HWND CreateListView(HWND hwndParent,int windowsID, vector<string> colNames,int x, int y, int w, int h, int size)
{
	INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);
	LV_COLUMN lvC;      // List View Column structure
	char szText[MAX_PATH];

	// Create the list-view window in report view with label editing enabled.
	HWND hWndListView = CreateWindow(WC_LISTVIEW,
		"Current CSV",
		WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_BORDER | LVS_SHOWSELALWAYS,
		x, y,w,h,
		hwndParent,
		(HMENU)windowsID,
		mainInst,
		NULL);
	// Add the columns.
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.fmt = LVCFMT_LEFT;  // left align the column
	lvC.pszText = szText;
	for (int index = 0; index < colNames.size(); index++)
	{
		lvC.cx = size;            // width of the column, in pixels
		lvC.iSubItem = index;
		lvC.pszText = (LPSTR)colNames[index].c_str();
		ListView_InsertColumn(hWndListView, index, &lvC);
	}
	SendMessage(hWndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	return (hWndListView);
}
//----------------------------------------------------------------------------------------------------------------
BOOL FillCustomerListViewItems(HWND hWndListView, vector<CRM::Customer> items)
{
	DateTime today;
	today.SetCurrentDateTime();
	string daysBack;
	string temp;
	int numDays = 0;
	//for (size_t index = 0; index < items.size(); index++)
	//the lsit view push items to the bottom of the list as you add them
	for (int index = items.size()-1; index >= 0; index--)
	{
		LVITEM lvI;

		// Initialize LVITEM members that are common to all items.
		lvI.mask = LVIF_TEXT | LVIF_PARAM; //// Initialize LV_ITEM members that are common to all items.
		lvI.stateMask = 0;
		lvI.state = 0;
		// structures
		
		lvI.mask=LVIF_TEXT;   // Text Style
		lvI.cchTextMax = 256; // Max size of test
		lvI.iItem=0;          // choose item  
		lvI.iSubItem=0;       // Put in first coluom
		lvI.pszText= (LPSTR)items[index].name.c_str(); // Text to display 

		SendMessage(hWndListView,LVM_INSERTITEM,0,(LPARAM)&lvI); // Send info to the Listview

		lvI.iSubItem=1;
		lvI.pszText = (LPSTR)items[index].email.c_str();
		SendMessage(hWndListView,LVM_SETITEM,0,(LPARAM)&lvI); // Enter text to SubItems

		lvI.iSubItem=2;
		temp = items[index].nextContactDate.ToString();
		lvI.pszText=(LPSTR)temp.c_str();
		SendMessage(hWndListView,LVM_SETITEM,0,(LPARAM)&lvI); // Enter text to SubItems

		lvI.iSubItem=3;
		temp = items[index].lastContactDate.ToString();
		lvI.pszText=(LPSTR)temp.c_str();
		SendMessage(hWndListView,LVM_SETITEM,0,(LPARAM)&lvI); // Enter text to SubItems

		lvI.iSubItem=4;
		if (today > items[index].nextContactDate)
		{
			numDays = today.day - items[index].nextContactDate.day;
			daysBack = to_string(numDays) + " days overdue";
		}
		else daysBack = "0";
		lvI.pszText=(LPSTR)daysBack.c_str();
		SendMessage(hWndListView,LVM_SETITEM,0,(LPARAM)&lvI); // Enter text to SubItems
	}
	
	return TRUE;
}
//----------------------------------------------------------------------------------------------------------------
BOOL FillCustomerConvoListViewItems(HWND hWndListView, vector<CRM::ContactDetails> items)
{
	//for (size_t index = 0; index < items.size(); index++)
	//the lsit view push items to the bottom of the list as you add them
	string temp;
	for (int index = items.size() - 1; index >= 0; index--)
	{
		LVITEM lvI;

		// Initialize LVITEM members that are common to all items.
		lvI.mask = LVIF_TEXT | LVIF_PARAM; //// Initialize LV_ITEM members that are common to all items.
		lvI.stateMask = 0;
		lvI.state = 0;
		// structures

		lvI.mask = LVIF_TEXT;   // Text Style
		lvI.cchTextMax = 256; // Max size of test
		lvI.iItem = 0;          // choose item  
		lvI.iSubItem = 0;       // Put in first coluom
		temp = items[index].date.ToString();
		lvI.pszText = (LPSTR)temp.c_str(); // Text to display 

		SendMessage(hWndListView, LVM_INSERTITEM, 0, (LPARAM)&lvI); // Send info to the Listview

		lvI.iSubItem = 1;
		lvI.pszText = (LPSTR)items[index].type.c_str();
		SendMessage(hWndListView, LVM_SETITEM, 0, (LPARAM)&lvI); // Enter text to SubItems

		lvI.iSubItem = 2;
		lvI.pszText = (LPSTR)items[index].notes.c_str();
		SendMessage(hWndListView, LVM_SETITEM, 0, (LPARAM)&lvI); // Enter text to SubItems

		lvI.iSubItem = 3;
		string temp;  
		if (items[index].successfulContact)
			temp = "yes";
		else
			temp = "no";
		lvI.pszText = (LPSTR)temp.c_str();
		SendMessage(hWndListView, LVM_SETITEM, 0, (LPARAM)&lvI); // Enter text to SubItems
	}

	return TRUE;
}
//----------------------------------------------------------------------------------------------------------------

// Message handler for main customer box.
INT_PTR CALLBACK CurCustomerView(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
	case WM_NOTIFY: //for the list view stuff
		return(CusCustomerNotifyHandler(hDlg, message, wParam, lParam));
		break;	

    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
		if (LOWORD(wParam) == IDC_IS_ACTIVE_CUSTOMER)
		{
			backEnd.curCustomer.active = IsDlgButtonChecked(curCustomerDialog, IDC_IS_ACTIVE_CUSTOMER);
		}

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
				backEnd.curCustomer.Clear();
				EndDialog(hDlg, LOWORD(wParam));
				RefreshMainWindowsContactList();
				return (INT_PTR)TRUE;
			}
        }
		if (LOWORD(wParam) == IDC_ADD_CONTACT)
        {
			InitAddCustomerContactDialog(true);
			return (INT_PTR)TRUE;
        }

		if (LOWORD(wParam) == IDCANCEL)
		{
			backEnd.curCustomer.Clear();
			EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
		}
        break;
    }
    return (INT_PTR)FALSE;
}
//----------------------------------------------------------------------------------------------------------------
// Message handler for main customer box.
INT_PTR CALLBACK AddCustomerContactView(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
		//set the state int he contact struct of the "is succesful" check box
		if (LOWORD(wParam) == IDC_IS_SUCCESSFUL_CONTACT)
		{
			backEnd.curContactDetails.successfulContact = IsDlgButtonChecked(addCustomerContactDialog, IDC_IS_SUCCESSFUL_CONTACT);
		}
		
		if (LOWORD(wParam) == IDOK)
        {
			FillCustomerContactStructFromContactDialog();
			if (!backEnd.SaveCurrentContactInfo())
			{
				MessageBox(NULL, "error saving contact details to database...try again?", NULL, NULL);
			}
			else
			{
				backEnd.curContactDetails.Clear();
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
        }

		if (LOWORD(wParam) == IDCANCEL)
		{
			backEnd.curCustomer.Clear();
			EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
		}
        break;
    }
    return (INT_PTR)FALSE;
}
//----------------------------------------------------------------------------------------------------------------
LRESULT MainWindowNotifyHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDM_LIST_VIEW_RESULTS)
		return 0L;

	LV_DISPINFO *pLvdi = (LV_DISPINFO *)lParam;

	switch (pLvdi->hdr.code)
	{
	case NM_DBLCLK:
	{
		// Get the first selected item
		int iPos = ListView_GetNextItem(mainListView, -1, LVNI_SELECTED);

		if (iPos != -1)
		{
			//char path[MAX_PATH];
			//ListView_GetItemText(mainListView, iPos, 0, path, MAX_PATH);
			backEnd.curCustomer = todaysContactList[iPos];
			InitCurCustomerDialog();
		}
	}
		break;

	/*case LVN_GETDISPINFO:

		switch (pLvdi->item.iSubItem)
		{
		case 0:     // Address
			//pLvdi->item.pszText = pHouse->szAddress;
			break;

		case 1:     // City
			//pLvdi->item.pszText = pHouse->szCity;
			break;

		default:
			break;
		}
		break;*/

	case LVN_BEGINLABELEDIT:
	{
		HWND hWndEdit;

		// Get the handle to the edit box.
		hWndEdit = (HWND)SendMessage(hWnd, LVM_GETEDITCONTROL,0, 0);
		// Limit the amount of text that can be entered.
		SendMessage(hWndEdit, EM_SETLIMITTEXT, (WPARAM)20, 0);
	}
	break;

	/*case LVN_ENDLABELEDIT:
		// Save the new label information
		if ((pLvdi->item.iItem != -1) && (pLvdi->item.pszText != NULL))
			//lstrcpy(pHouse->szAddress, pLvdi->item.pszText);
			lstrcpy((LPSTR)"is you is oris you aint", pLvdi->item.pszText);
		break;

	case LVN_COLUMNCLICK:
		// The user clicked on one of the column headings - sort by
		// this column.
		//ListView_SortItems(pNm->hdr.hwndFrom,
		//	ListViewCompareProc,
		//	(LPARAM)(pNm->iSubItem));
		break;*/

	default:
		break;
	}
	return 0L;
}
//----------------------------------------------------------------------------------------------------------------
LRESULT CusCustomerNotifyHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDC_CONVO_LIST_VIEW)
		return 0L;

	LV_DISPINFO* pLvdi = (LV_DISPINFO*)lParam;

	switch (pLvdi->hdr.code)
	{
	case NM_DBLCLK:
	{
		// Get the first selected item
		int iPos = ListView_GetNextItem(convoListView, -1, LVNI_SELECTED);

		if (iPos != -1)
		{
			backEnd.curContactDetails = curCustomerConversationList[iPos];
			InitAddCustomerContactDialog(false);
		}
	}
	break;

	default:
		break;
	}
	return 0L;
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