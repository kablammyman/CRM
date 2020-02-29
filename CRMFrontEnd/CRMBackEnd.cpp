#include "stdafx.h"
#include "CRMBackEnd.h"

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
}
void CRMBackEnd::DoPhoneLookup(string phone )
{
	curContact = crm->GetCustomerByPhone(phone);
}

void CRMBackEnd::SaveCurrentInfo()
{

}