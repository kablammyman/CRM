#include "stdafx.h"
#include "CRMBackEnd.h"

void CRMBackEnd::SetDB(string dbPath)
{
	//crm = new CRM("D:\\source\\CRM\\CRM.db");
	
	if(crm == nullptr)
		crm = new CRM(dbPath);
	else
	{
		delete crm;
		crm = nullptr;
		crm = new CRM(dbPath);
	}
}

CRMBackEnd::CRMBackEnd(string dbPath)
{
	//crm = new CRM("D:\\source\\CRM\\CRM.db");
	SetDB(dbPath);
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
	{
		return crm->AddNewCustomerEntry(curCustomer);
	}
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
	vector<CRM::Customer> cust;
	crm->GetAllContacts(cust);
	for(size_t i = 0; i < cust.size(); i++)
	{
		crm->AddCustomerToTagDB(cust[i]);
	}

	list = crm->GetTodaysContacts();
	vector<CRM::Customer> overdue = crm->GetOverdueContacts();
	for (int i = 0; i < overdue.size(); i++)
	{
		list.push_back(overdue[i]);
	}
}

void CRMBackEnd::FillCurCusterContactList(int customerID, vector<CRM::ContactDetails>& list)
{
	list = crm->GetAllCustomerContact(customerID);	
}

void CRMBackEnd::AddTags(string csvLine)
{
	vector<string> newTags = StringUtils::Tokenize(csvLine,",");
	crm->AddTagsToCustomer(curCustomer.id,newTags);
}

vector<string> CRMBackEnd::GetAllTagsForCustomer(int curCustomerID)
{
	return crm->GetAllCustomerTags(curCustomerID);
}

void CRMBackEnd::GetAllCustersThatHasTag(string tag,vector<CRM::Customer> &list)
{
	list = 	crm->GetAllCustomersWithTag(tag);
}

void CRMBackEnd::GetAllCustersThatHasAllTheseTag(string tagsListString,vector<CRM::Customer> &list)
{
	vector<string> tagList = StringUtils::Tokenize(tagsListString,",");
	list = 	crm->GetAllCustomersWithTheseTags(tagList);
}

