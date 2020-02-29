#pragma once
#include "CRM.h"
#include <string>

using namespace std;

class CRMBackEnd
{
	CRM *crm;
	
public:
	CRM::Contact curContact;
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

	//CRM::Contact contact= crm.GetContact("schwheatbaybeeghrey@gmail.com");
	//cout << contact.name << endl;
	
	//crm.DeactivateOldCustomers();
	
	return 0;
}

*/