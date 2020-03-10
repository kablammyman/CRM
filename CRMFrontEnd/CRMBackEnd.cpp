#include "stdafx.h"
#include "CRMBackEnd.h"

/*int test()
{
	CRM crm("D:\\source\\CRM\\CRM.db");

	//CRM::Customer customer= crm.GetContact("schwheatbaybeeghrey@gmail.com");
	//cout << customer.name << endl;
	
	//crm.DeactivateOldCustomers();
	
	return 0;
}*/

CRMBackEnd::CRMBackEnd()
{
	crm = new CRM("D:\\source\\CRM\\CRM.db");
}
void CRMBackEnd::DoEmailLookup(string email )
{
	curCustomer = crm->GetCustomerByEmail(email);
}
void CRMBackEnd::DoNameLookup(string name)
{
	curCustomer = crm->GetCustomerByName(name);
}
void CRMBackEnd::DoPhoneLookup(string phone )
{
	curCustomer = crm->GetCustomerByPhone(phone);
}

bool CRMBackEnd::SaveCurrentInfo()
{
	if (curCustomer.id < 1)//new entry
		return crm->AddNewCustomerEntry(curCustomer);
	else
		return crm->UpdateCustomerEntry(curCustomer);
}

bool CRMBackEnd::SaveCurrentContactInfo()
{
	//int customerID, string contactType, string notes, bool successful
	return crm->AddNewContactEntry(curContactDetails);
}

void CRMBackEnd::FillTodaysCusterContactList(vector<CRM::Customer> &list)
{
	list = crm->GetTodaysContacts();
	vector<CRM::Customer> overdue = crm->GetOverdueContacts();
	for (int i = 0; i < overdue.size(); i++)
	{
		list.push_back(overdue[i]);
	}
}