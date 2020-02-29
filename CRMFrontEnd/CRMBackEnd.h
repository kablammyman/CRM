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
	void SaveCurrentInfo();
};
/*
int test()
{
	CRM crm("D:\\source\\CRM\\CRM.db");

	//CRM::Customer customer= crm.GetContact("schwheatbaybeeghrey@gmail.com");
	//cout << customer.name << endl;
	
	//crm.DeactivateOldCustomers();
	
	return 0;
}

*/