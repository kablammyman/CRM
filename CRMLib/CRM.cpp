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
void CRM::FillSingleCatactFromDBResults(vector <DatabaseController::DBResult> &dbResults, CRM::Contact &contact,int index = 0)
{
	if (dbResults.size() <= index)
		return;
	contact.name = dbResults[index].GetValueFromKey("Name");
	contact.email = dbResults[index].GetValueFromKey("Email");
	contact.phone = dbResults[index].GetValueFromKey("Phone");
	contact.notes = dbResults[index].GetValueFromKey("Notes");
	contact.nextContactDate = DateTime(dbResults[index].GetValueFromKey("NextContactDate"));
	contact.lastContactDate = DateTime(dbResults[index].GetValueFromKey("LastContactDate"));
	contact.lastContactDate = DateTime(dbResults[index].GetValueFromKey("DateAdded"));
	contact.numContacts = dbResults[index].GetIntValueFromKey("NumContacts");
	contact.active = dbResults[index].GetIntValueFromKey("Active");//hopefully this works...1 = true, 0 will be false

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
void  CRM::UpdateCustomerEntry(CRM::Contact & contact)
{
	
	string output;

	vector<DatabaseController::dbDataPair> insertData;
	insertData.push_back(make_pair("Name", contact.name));
	insertData.push_back(make_pair("Email", contact.email));
	insertData.push_back(make_pair("Phone", contact.phone));
	insertData.push_back(make_pair("LastContactDate",DateTime::Now()));
	insertData.push_back(make_pair("NumContacts","NumContacts = NumContacts + 1"));
	insertData.push_back(make_pair("NextContactDate", contact.nextContactDate.ToString()));
	insertData.push_back(make_pair("Notes", contact.notes));

	DatabaseController::dbDataPair whereData = make_pair("ID",to_string(contact.id));
	
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
vector<CRM::Contact> CRM::GetTodaysContacts()
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
	
	vector<Contact> todaysContacts;
	for (int i = 0; i < dbResults.size(); i++)
	{
		Contact contact;
		FillSingleCatactFromDBResults(dbResults, contact,i);
		todaysContacts.push_back(contact);
	}

	return 	todaysContacts;
}
//----------------------------------------------------------------------------------
//later combine these 2 methods to use all the main logic in one place
vector<CRM::Contact> CRM::GetAllActiveContacts()
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

	vector<Contact> activeContacts;
	for (int i = 0; i < dbResults.size(); i++)
	{
		/*DateTime next(dbResults[i].GetValueFromKey("NextContactDate"));
		if(next == today)*/
		Contact contact;
		FillSingleCatactFromDBResults(dbResults, contact,i);
		activeContacts.push_back(contact);
	}

	return 	activeContacts;
}
//----------------------------------------------------------------------------------
CRM::Contact CRM::GetContactByEmail(string email)
{
	string output;
	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("Email",email));
	
	if (!dbController.DoDBQuerey("CRM", contactQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error getting contact from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, contactQuereyList, dbResults);

	Contact contact;
	FillSingleCatactFromDBResults(dbResults, contact);
	return contact;
}
//----------------------------------------------------------------------------------
CRM::Contact CRM::GetContactByPhone(string phone)
{
	vector<string> quereyList = { "ID","Name","Email","Phone","Active","LastContactDate","NextContactDate","Notes" };
	string output;
	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("Phone",phone));
	
	if (!dbController.DoDBQuerey("CRM", quereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error getting contact from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, quereyList, dbResults);

	Contact contact;
	FillSingleCatactFromDBResults(dbResults, contact);
	return contact;
}
//----------------------------------------------------------------------------------
CRM::Contact CRM::GetContactByID(int id)
{
	vector<string> quereyList = { "ID","Name","Email","Phone","Active","LastContactDate","NextContactDate","NumContacts","Notes" };
	string output;
	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("ID",to_string(id)));
	
	if (!dbController.DoDBQuerey("CRM", quereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error getting contact from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, quereyList, dbResults);

	Contact contact;
	FillSingleCatactFromDBResults(dbResults, contact);
	return contact;
}
//----------------------------------------------------------------------------------
void CRM::DeleteContact(CRM::Contact &contact)
{
	string querey = "DELETE FROM CRM WHERE ID = " + contact.id;
	string output;
	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error deleting contact from CRM DB: " + output);
	}

	contact.id = -1;
	contact.name.clear();
	contact.email.clear();
	contact.phone.clear();
}
//----------------------------------------------------------------------------------
void CRM::DeleteContactByEmail(string email)
{
	string querey = "DELETE FROM CRM WHERE Email = \"" + email + "\"";
	string output;
	if(!dbController.DoDBQuerey(querey,output))
	{
		throw string( "error deleting contact from CRM DB: " + output);
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
					throw string( "error setting contact to inactive: " + output);
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