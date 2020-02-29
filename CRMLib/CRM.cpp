// CRM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream> //for exporting data

#include "CRM.h"
#include "StringUtils.h"
#include "DatabaseController.h"
#include "DateTime.h"

CRM::CRM(string dbPath)
{
	dbController.OpenDB(dbPath);
	contactQuereyList = { "ID","Name","Email","Phone","Active","LastContactDate","NextContactDate","Notes","DateAdded" };
}
void CRM::DebugPrint(string message)
{
	if (verbose)
		cout << message << endl;
}
//----------------------------------------------------------------------------------
void CRM::FillSingleCustomerFromDBResults(vector <DatabaseController::DBResult> &dbResults, CRM::Customer &customer,int index = 0)
{
	if (dbResults.size() <= index)
		return;
	customer.name = dbResults[index].GetValueFromKey("Name");
	customer.email = dbResults[index].GetValueFromKey("Email");
	customer.phone = dbResults[index].GetValueFromKey("Phone");
	customer.notes = dbResults[index].GetValueFromKey("Notes");
	customer.nextContactDate = DateTime(dbResults[index].GetValueFromKey("NextContactDate"));
	customer.lastContactDate = DateTime(dbResults[index].GetValueFromKey("LastContactDate"));
	customer.lastContactDate = DateTime(dbResults[index].GetValueFromKey("DateAdded"));
	customer.numContacts = dbResults[index].GetIntValueFromKey("NumContacts");
	customer.active = dbResults[index].GetIntValueFromKey("Active");//hopefully this works...1 = true, 0 will be false

}
//----------------------------------------------------------------------------------
void CRM::AddNewEntryoCRM(string name, string email, string phone, string notes)
{
	//first, update the status in the db
	string querey = "SELECT Email FROM CRM WHERE Email = \"" + email + "\"";
	string output;


	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string(  "error gettting advertising stats: "+ output);
	}	
		
	if (!output.empty())
	{
		DebugPrint("we already have " + email);
		return;
	}

	DateTime tomorrowDate;
	tomorrowDate.SetCurrentDateTime();

	DateTime todaysPost;
	todaysPost.SetCurrentDateTime();

	DateTime tomorrowsPost;
	tomorrowsPost.SetCurrentDateTime();

	vector<DatabaseController::dbDataPair> insertData;
	insertData.push_back(make_pair("Name", name));
	insertData.push_back(make_pair("Email", email));
	insertData.push_back(make_pair("Phone", phone));
	insertData.push_back(make_pair("NumContacts", "1"));
	insertData.push_back(make_pair("LastContactDate",todaysPost.ToString()));
	insertData.push_back(make_pair("NextContactDate", tomorrowsPost.ToString()));
	insertData.push_back(make_pair("Notes", notes));
	insertData.push_back(make_pair("Active", "1"));
	insertData.push_back(make_pair("DateAdded", todaysPost.ToString()));

		
	output.clear();
	if(!dbController.InsertNewDataEntry("CRM",  insertData, output))
	{
		if(dbController.GetLastError() != "")
		{
			throw string("error adding new value into CRM DB: " + dbController.GetLastError());
		}
	}

	DebugPrint("added: " + name + " email: " + email);
}
//----------------------------------------------------------------------------------
//after each interaction with a customer, call this method
void  CRM::UpdateCustomerEntry(CRM::Customer & customer)
{
	
	string output;

	vector<DatabaseController::dbDataPair> insertData;
	insertData.push_back(make_pair("Name", customer.name));
	insertData.push_back(make_pair("Email", customer.email));
	insertData.push_back(make_pair("Phone", customer.phone));
	insertData.push_back(make_pair("LastContactDate",DateTime::Now()));
	insertData.push_back(make_pair("NumContacts",to_string(customer.numContacts)));
	insertData.push_back(make_pair("NextContactDate", customer.nextContactDate.ToString()));
	insertData.push_back(make_pair("Notes", customer.notes));

	DatabaseController::dbDataPair whereData = make_pair("ID",to_string(customer.id));
	
	if (!dbController.UpdateEntry("CRM", insertData, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error updating person in CRM DB: " + dbController.GetLastError());
		}
	}
}
//----------------------------------------------------------------------------------
//after each interaction with a customer, call this method
void CRM::UpdateContactAndNotes(int ID, string notes)
{
	//first, update the status in the db
	string output;
	string newNotes;
	if (!notes.empty())
		newNotes = notes;
	else
		newNotes = "contacted";

	string notesString =  "notes = notes ||\n\"" + DateTime::Now() + ": " +newNotes;

	string querey = "UPDATE CRM SET NumContacts = NumContacts + 1, " + notesString + " WHERE ID = " + to_string(ID);
	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error updating entry in CRM DB: " + output);
	}
}
//----------------------------------------------------------------------------------
vector<CRM::Customer> CRM::GetTodaysContacts()
{
	DateTime todaysPost;
	todaysPost.SetCurrentDateTime();
	string output;

	//used for testing
	//tomorrowDate = datetime.datetime.now() + datetime.timedelta(days=1)
	//todaysPost = tomorrowDate.strftime("%Y %m %d")

	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("NextContactDate",todaysPost.ToString()));
	whereData.push_back(make_pair("Active","1"));
	
	if (!dbController.DoDBQuerey("CRM", contactQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string(" error getting baisc info: " + dbController.GetLastError());
		}
	}

	/*
	//the oly python way converted
	string quereyString = StringUtils::FlattenVector(quereyList); 
	string querey = "SELECT " + quereyString + " FROM CRM WHERE NextContactDate = \"" + todaysPost.ToString() + "\" AND Active = 1";
	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error adding new value into CRM DB: " + dbController.GetLastError());
	}*/
	
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, contactQuereyList, dbResults);
	
	vector<Customer> todaysContacts;
	for (int i = 0; i < dbResults.size(); i++)
	{
		Customer customer;
		FillSingleCustomerFromDBResults(dbResults, customer,i);
		todaysContacts.push_back(customer);
	}

	return 	todaysContacts;
}
//----------------------------------------------------------------------------------
//later combine these 2 methods to use all the main logic in one place
vector<CRM::Customer> CRM::GetAllActiveContacts()
{
	//DeactivateOldCustomers()
	
	DateTime todaysPost;
	todaysPost.SetCurrentDateTime();
	string output;

	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("Active","1"));
	
	if (!dbController.DoDBQuerey("CRM", contactQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string(" error getting baisc info: " + dbController.GetLastError());
		}
	}
	
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, contactQuereyList, dbResults);
	/*DateTime today;
	today.SetCurrentDateTime();*/

	vector<Customer> activeContacts;
	for (int i = 0; i < dbResults.size(); i++)
	{
		/*DateTime next(dbResults[i].GetValueFromKey("NextContactDate"));
		if(next == today)*/
		Customer customer;
		FillSingleCustomerFromDBResults(dbResults, customer,i);
		activeContacts.push_back(customer);
	}

	return 	activeContacts;
}
//----------------------------------------------------------------------------------
CRM::Customer CRM::GetCustomerByEmail(string email)
{
	string output;
	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("Email",email));
	
	if (!dbController.DoDBQuerey("CRM", contactQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error getting customer from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, contactQuereyList, dbResults);

	Customer customer;
	FillSingleCustomerFromDBResults(dbResults, customer);
	return customer;
}
//----------------------------------------------------------------------------------
CRM::Customer CRM::GetCustomerByPhone(string phone)
{
	
	string output;
	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("Phone",phone));
	
	if (!dbController.DoDBQuerey("CRM", contactQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error getting customer from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, contactQuereyList, dbResults);

	Customer customer;
	FillSingleCustomerFromDBResults(dbResults, customer);
	return customer;
}
//----------------------------------------------------------------------------------
CRM::Customer CRM::GetCustomerByID(int id)
{
	vector<string> quereyList = { "ID","Name","Email","Phone","Active","LastContactDate","NextContactDate","NumContacts","Notes" };
	string output;
	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("ID",to_string(id)));
	
	if (!dbController.DoDBQuerey("CRM", quereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error getting customer from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, quereyList, dbResults);

	Customer customer;
	FillSingleCustomerFromDBResults(dbResults, customer);
	return customer;
}
//----------------------------------------------------------------------------------
void CRM::DeleteContact(CRM::Customer &customer)
{
	string querey = "DELETE FROM CRM WHERE ID = " + customer.id;
	string output;
	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error deleting customer from CRM DB: " + output);
	}

	customer.id = -1;
	customer.name.clear();
	customer.email.clear();
	customer.phone.clear();
}
//----------------------------------------------------------------------------------
void CRM::DeleteContactByEmail(string email)
{
	string querey = "DELETE FROM CRM WHERE Email = \"" + email + "\"";
	string output;
	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error deleting customer from CRM DB: " + output);
	}
}
//----------------------------------------------------------------------------------		
void CRM::SetActiveStatus(int ID, int activeStatus)
{
	string querey = "UPDATE CRM SET Active = " + to_string(activeStatus) + "WHERE ID = " + to_string(ID);
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error setting active status CRM DB: " + output);
	}
	
}
//----------------------------------------------------------------------------------
int CRM::GetActiveStatus(int ID)
{
	string querey = "SELECT Active FROM CRM WHERE ID = " + to_string(ID);
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error getting active status CRM DB: " + output);
	}

	if (output.empty())
		return -1;
	DatabaseController::RemoveTableNameFromOutput(output);
	if(!output.empty())
		return stoi(output);
	return -1;
}
//----------------------------------------------------------------------------------
void CRM::DeactivateOldCustomers()
{
	string querey = "SELECT ID, Active, LastContactDate, NextContactDate FROM CRM";
	vector<string> quereyList = { "ID", "Active", "LastContactDate", "NextContactDate" };
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error getting all contacts: " + output);
	}

	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, quereyList, dbResults);
	string updateStatement = "UPDATE CRM SET Active = 0 WHERE ID = ";
	for (int i = 0; i < dbResults.size(); i++)
	{
		if (dbResults[i].GetValueFromKey("Active") == "1")
		{
			DateTime curDate;
			curDate.SetCurrentDateTime();

			DateTime lastContact(dbResults[0].GetValueFromKey("LastContactDate"));
			DateTime nextContact(dbResults[0].GetValueFromKey("NextContactDate"));
			
			if ( curDate.TimeDiff(lastContact)  >= 14 && curDate > nextContact)
			{
				if(!dbController.DoDBQuerey(updateStatement + dbResults[i].GetValueFromKey("ID"),output))
				{
					throw string( "error setting customer to inactive: " + output);
				}
			}
		}
	}
}
//----------------------------------------------------------------------------------
void CRM::ExportAllData()
{
	string querey = "SELECT Name,Email,Phone,ID FROM CRM";
	vector<string> quereyList = { "Name", "Email", "Phone", "ID" };
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error getting all contacts: " + output);
	}

	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, quereyList, dbResults);

	ofstream  csv("crmEmailExport.csv");
	csv << "ID,name,email,phone\n";

	for (int i = 0; i < dbResults.size(); i++)
	{
		csv << dbResults[i].GetValueFromKey("ID") << ",";
		csv << dbResults[i].GetValueFromKey("Name") << ","; 
		csv << dbResults[i].GetValueFromKey("Email")<< ","; 
		csv << dbResults[i].GetValueFromKey("Phone")<< "," << endl;
	}
	csv.close();
}
//----------------------------------------------------------------------------------
void CRM::IncNumContacts(int ID)
{
	string querey = "UPDATE CRM SET NumContacts = NumContacts + 1, LastContactDate = " + DateTime::Now() + " WHERE ID = "+ to_string(ID);
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error incrementing num contacts CRM DB: " + output);
	}
}
//----------------------------------------------------------------------------------
void CRM::UpdateNotes(int ID, string notes)
{
	string querey = "UPDATE CRM SET notes = notes || \"\n" + DateTime::Now() + ":" + notes + "\n\" WHERE ID  " + to_string(ID);
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error updateing notes CRM DB: " + output);
	}
}
//----------------------------------------------------------------------------------
void CRM::AddNewContactEntry(int customerID, string contactType, string notes, bool successful)
{
	string output;

	DateTime todaysPost;
	todaysPost.SetCurrentDateTime();
	
	vector<DatabaseController::dbDataPair> insertData;
	insertData.push_back(make_pair("CustomerID", to_string(customerID)));
	insertData.push_back(make_pair("Type", contactType));
	insertData.push_back(make_pair("Successful", to_string(successful)));
	insertData.push_back(make_pair("notes", notes));
	insertData.push_back(make_pair("Date",todaysPost.ToString()));

	output.clear();

	if(!dbController.InsertNewDataEntry("ContactDetails",  insertData, output))
	{
		if(dbController.GetLastError() != "")
		{
			throw string("error adding new value into CRM DB: " + dbController.GetLastError());
		}
	}
}