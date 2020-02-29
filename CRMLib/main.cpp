#include "stdafx.h"
#include "CRM.h"
#include <iostream>

using namespace std;

int test(int argc, char* argcv[])
{
	CRM crm("D:\\source\\CRM\\CRM.db");

	//CRM::Contact contact= crm.GetContact("schwheatbaybeeghrey@gmail.com");
	//cout << contact.name << endl;
	
	//crm.DeactivateOldCustomers();
	
	// Options

	/*parser.add_option("--getActiveContacts", action='store_true', help="print out all the people who we are in current contact with")
	parser.add_option("--getTodaysContacts",action='store_true', help="get the people we need to contact today")
	parser.add_option("--exportAllEmail",action="store_true", help="optonal param to export emails and names into a csv")
	
	
	if ( options.getActiveContacts)
		crm.GetAllActiveContacts();

	if ( options.getTodaysContacts)
		crm.GetTodaysContacts();
	
	if ( options.exportAllEmail)
		crm.ExportAllData();*/
	return 0;
}