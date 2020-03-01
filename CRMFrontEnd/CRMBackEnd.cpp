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
	curContact = crm->GetCustomerByEmail(email);
}
void CRMBackEnd::DoNameLookup(string name)
{
	curContact = crm->GetCustomerByName(name);
}
void CRMBackEnd::DoPhoneLookup(string phone )
{
	curContact = crm->GetCustomerByPhone(phone);
}

bool CRMBackEnd::SaveCurrentInfo()
{
	if (curContact.id < 1)//new entry
		return crm->AddNewCustomerEntry(curContact);
	else
		return crm->UpdateCustomerEntry(curContact);
}