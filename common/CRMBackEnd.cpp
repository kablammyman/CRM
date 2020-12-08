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

void CRMBackEnd::AddTags(vector<string> &newTags)
{
	crm->AddTagsToCustomer(curCustomer.id, newTags);
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

string CRMBackEnd::FindNameInMessage(string message)
{
	StringUtils::ToUpper(message);
	string name;
	//vector<string> searchKeys = { "HELLO ","THIS IS ","MY NAME IS " };
	vector<string> delims = { ",",".","AND"," I " };
	string key = "MY NAME IS ";
	size_t index = message.find(key);
	if (index != string::npos)
	{
		//these are the ways I see the statment "my name is " ended, 
		//so i look for one of these so i can get the full name if its provided
		size_t nearestDelm = string::npos;
		for (size_t i = 0; i < delims.size(); i++)
		{
			size_t curDelim = message.find(delims[i], index);
			if (curDelim < nearestDelm)
				nearestDelm = curDelim;
		}
		index += key.size();
		if((nearestDelm - index) < 25)
			name = StringUtils::StringClean(message.substr(index, nearestDelm - index), true);
		StringUtils::ToProperNoun(name);
	}
	return name;
}
string CRMBackEnd::FindEmailInMessage(string message)
{
	size_t found = message.find("@");
	size_t start = found, end = found;
	string email;
	if (found != string::npos)
	{
		//first get the end of the email address part
		while (start > 0 &&  message[start] != ' ' && message[start] != '\n' && message[start] != '\t')
		{
			start--;
		}

		//next get the end of the email address part
		while (end < message.size() && message[end] != ' ' && message[end] != '\n' && message[end] != '\t')
		{
			end++;
		}

		email = StringUtils::StringClean(message.substr(start, end - start),true);
	}
	return email;
}
void CRMBackEnd::FindTagsInText(string message,vector<string>& foundTags)
{
	StringUtils::ToUpper(message);
	//right now, im only looking for the url i send them OR some other indcator about where the lot is they are intersted in
	vector<string> searchKeys = { "VICTORSVACANTLAND.COM/PARCELS/", "BURLESON","CALDWELL","HENDERSON","KEY RANCH","MAGNOLIA","FOUNTAIN", };
	bool add = false;

	string tag = StringUtils::GetWordFromListInLine(searchKeys, message,false);
	
	if (!tag.empty())
	{
		add = true;
		//if i sent them a url, find out the county within the url
		if (tag == "VICTORSVACANTLAND.COM/PARCELS/")
		{
			size_t start = message.find(tag);
			start += tag.size(); //(strlen of front part of url)
			size_t end = message.find("/",start);
			tag = message.substr(start, end - start);
		}
		else
		{
			for (size_t i = 0; i < foundTags.size(); i++)
				if (foundTags[i] == tag)
					add = false;
		}
	}
	if (add)
		foundTags.push_back(tag);
}
void CRMBackEnd::AddConvosToCurrentCustomer(vector<CRM::ContactDetails>& convos)
{
	for(size_t i = 0; i < convos.size(); i++)
		crm->AddNewContactEntry(curContactDetails);
}