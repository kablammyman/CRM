#pragma once
#include "CRM.h"
#include <string>
#include <vector>

using namespace std;

class CRMBackEnd
{
	CRM *crm;
	
public:
	CRM::Customer curCustomer;
	CRM::ContactDetails curContactDetails;
	CRMBackEnd(){}
	CRMBackEnd(string dbPath);
	void SetDB(string dbPath);
	void DoEmailLookup(string email);
	void DoNameLookup(string name);
	void DoPhoneLookup(string phone);
	bool SaveCurrentInfo();
	bool SaveCurrentContactInfo();
	void FillTodaysCusterContactList(vector<CRM::Customer> &list);
	void FillCurCusterContactList(int customerID, vector<CRM::ContactDetails>& list);
	void AddTags(string csvLine);
	void AddTags(vector<string>& newTags);
	vector<string> GetAllTagsForCustomer(int curCustomerID);
	void GetAllCustersThatHasTag(string tag,vector<CRM::Customer> &list);
	void GetAllCustersThatHasAllTheseTag(string tagsListString,vector<CRM::Customer> &list);
};