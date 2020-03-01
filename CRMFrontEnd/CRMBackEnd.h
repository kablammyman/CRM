#pragma once
#include "CRM.h"
#include <string>

using namespace std;

class CRMBackEnd
{
	CRM *crm;
	
public:
	CRM::Customer curContact;
	CRMBackEnd();
	void DoEmailLookup(string email);
	void DoNameLookup(string name);
	void DoPhoneLookup(string phone);
	bool SaveCurrentInfo();
};
