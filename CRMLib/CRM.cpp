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
	customerQuereyList = { "ID","Name","Email","Phone","Active","LastContactDate","NextContactDate","Notes","DateAdded","UniqueName" };
	contactQuereyList = { "ID","CustomerID","Date","Type","Successful","notes" };
	tags.SetDBController(&dbController);
	tags.CreateTagTables();
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

	customer.id = dbResults[index].GetIntValueFromKey("ID");
	customer.name = dbResults[index].GetValueFromKey("Name");
	customer.email = dbResults[index].GetValueFromKey("Email");
	customer.phone = dbResults[index].GetValueFromKey("Phone");
	customer.uniqueName = dbResults[index].GetValueFromKey("UniqueName");
	customer.notes = dbResults[index].GetValueFromKey("Notes");
	customer.nextContactDate = DateTime(dbResults[index].GetValueFromKey("NextContactDate"));
	customer.lastContactDate = DateTime(dbResults[index].GetValueFromKey("LastContactDate"));
	customer.lastContactDate = DateTime(dbResults[index].GetValueFromKey("DateAdded"));
	customer.numContacts = dbResults[index].GetIntValueFromKey("NumContacts");
	customer.active = dbResults[index].GetIntValueFromKey("Active");//hopefully this works...1 = true, 0 will be false

}
//----------------------------------------------------------------------------------
void CRM::FillSingleContactDetailFromDBResults(vector <DatabaseController::DBResult>& dbResults, CRM::ContactDetails& contact, int index = 0)
{
	if (dbResults.size() <= index)
		return;

	contact.id = dbResults[index].GetIntValueFromKey("ID");
	contact.customerID = dbResults[index].GetIntValueFromKey("CustomerID");
	contact.type = dbResults[index].GetValueFromKey("Type");
	contact.successfulContact = dbResults[index].GetIntValueFromKey("Successful");
	contact.notes = dbResults[index].GetValueFromKey("notes");
	contact.date = DateTime(dbResults[index].GetValueFromKey("Date"));
}
//----------------------------------------------------------------------------------
bool CRM::AddNewCustomerEntry(CRM::Customer & customer)
{
	//first, update the status in the db
	string output;
	if (!customer.email.empty())
	{
		string querey = "SELECT Email FROM CRM WHERE Email = \"" + customer.email + "\"";


		if (!dbController.DoDBQuerey(querey, output))
		{
			//throw string(  "error gettting advertising stats: "+ output);
			DebugPrint("error gettting advertising stats: " + output);
			return false;
		}

		if (!output.empty())
		{
			DebugPrint("we already have " + customer.email);
			return false;
		}
	}
	DateTime todaysPost;
	todaysPost.SetCurrentDateTime();

	vector<DatabaseController::dbDataPair> insertData;
	insertData.push_back(make_pair("Name", customer.name));
	insertData.push_back(make_pair("Email", customer.email));
	insertData.push_back(make_pair("Phone", customer.phone));
	insertData.push_back(make_pair("NumContacts", "1"));
	insertData.push_back(make_pair("LastContactDate",todaysPost.ToString()));
	insertData.push_back(make_pair("NextContactDate", customer.nextContactDate.ToString()));
	insertData.push_back(make_pair("Notes", customer.notes));
	insertData.push_back(make_pair("Active", "1"));
	insertData.push_back(make_pair("DateAdded", todaysPost.ToString()));
	insertData.push_back(make_pair("UniqueName", customer.uniqueName));

		
	output.clear();
	if(!dbController.InsertNewDataEntry("CRM",  insertData, output))
	{
		if(dbController.GetLastError() != "")
		{
			//throw string("error adding new value into CRM DB: " + dbController.GetLastError());
			DebugPrint("error adding new value into CRM DB: " + dbController.GetLastError());
			return false;
		}
	}
	customer.id = dbController.GetLatestRowID();
	DebugPrint("added: " + customer.name + " email: " + customer.email);

	//now add the entry for the tag tables
	tags.AddNewItem(customer.name,customer.uniqueName, customer.id );
	return true;
}
//----------------------------------------------------------------------------------
//after each interaction with a customer, call this method
bool  CRM::UpdateCustomerEntry(CRM::Customer & customer)
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
	insertData.push_back(make_pair("Active", to_string(customer.active)));
	insertData.push_back(make_pair("UniqueName", customer.uniqueName));

	DatabaseController::dbDataPair whereData = make_pair("ID",to_string(customer.id));
	
	if (!dbController.UpdateEntry("CRM", insertData, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			//throw string("error updating person in CRM DB: " + dbController.GetLastError());
			DebugPrint("error updating person in CRM DB: " + dbController.GetLastError());
			return false;
		}
	}
	return true;
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
	
	if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint(" error getting baisc info: " + dbController.GetLastError());
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
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);
	
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
vector<CRM::Customer> CRM::GetOverdueContacts()
{
	DateTime today;
	today.SetCurrentDateTime();
	string output;

	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("Active","1"));
	
	if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint(" error getting baisc info: " + dbController.GetLastError());
		}
	}

	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);
	
	vector<Customer> todaysContacts;
	for (int i = 0; i < dbResults.size(); i++)
	{
		Customer customer;
		FillSingleCustomerFromDBResults(dbResults, customer,i);

		if(today > customer.nextContactDate)
			todaysContacts.push_back(customer);
	}

	return 	todaysContacts;
}
//----------------------------------------------------------------------------------
//later combine these 2 methods to use all the main logic in one place
void CRM::GetAllContacts(vector<CRM::Customer> &customers)
{
	string output;

	if (!dbController.DoDBQuerey("CRM", customerQuereyList, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint(" error getting baisc info: " + dbController.GetLastError());
		}
	}

	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);

	for (int i = 0; i < dbResults.size(); i++)
	{
		Customer customer;
		FillSingleCustomerFromDBResults(dbResults, customer,i);
		customers.push_back(customer);
	}
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
	
	if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint(" error getting baisc info: " + dbController.GetLastError());
		}
	}
	
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);
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

	DatabaseController::dbDataPair whereData = make_pair("Email",email);
	
	if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint("error getting customer from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);

	Customer customer;
	FillSingleCustomerFromDBResults(dbResults, customer);
	return customer;
}
//----------------------------------------------------------------------------------
CRM::Customer CRM::GetCustomerByName(string name)
{
	string output;
	
	DatabaseController::dbDataPair whereData = make_pair("Name",name);
	
	if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			throw string("error getting customer from CRM DB: " + dbController.GetLastError());
		}
	}
	if (output.empty())
	{
		whereData.first = "UniqueName";
		if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
		{
			if (dbController.GetLastError() != "")
			{
				DebugPrint("error getting customer from CRM DB: " + dbController.GetLastError());
			}
		}
	}

	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);

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
	
	if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint("error getting customer from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);

	Customer customer;
	customer.id = -1;
	FillSingleCustomerFromDBResults(dbResults, customer);
	return customer;
}
//----------------------------------------------------------------------------------
CRM::Customer CRM::GetCustomerByID(int id)
{
	string output;
	
	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("ID",to_string(id)));
	
	if (!dbController.DoDBQuerey("CRM", customerQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint("error getting customer from CRM DB: " + dbController.GetLastError());
		}
	}
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);

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
		DebugPrint( "error deleting customer from CRM DB: " + output);
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
		DebugPrint( "error deleting customer from CRM DB: " + output);
	}
}
//----------------------------------------------------------------------------------		
void CRM::SetActiveStatus(int ID, int activeStatus)
{
	string querey = "UPDATE CRM SET Active = " + to_string(activeStatus) + "WHERE ID = " + to_string(ID);
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		DebugPrint( "error setting active status CRM DB: " + output);
	}
	
}
//----------------------------------------------------------------------------------
int CRM::GetActiveStatus(int ID)
{
	string querey = "SELECT Active FROM CRM WHERE ID = " + to_string(ID);
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		DebugPrint( "error getting active status CRM DB: " + output);
		return -1;
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

	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		DebugPrint( "error getting all contacts: " + output);
	}

	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, customerQuereyList, dbResults);
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
					DebugPrint( "error setting customer to inactive: " + output);
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
		DebugPrint( "error getting all contacts: " + output);
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
		DebugPrint( "error incrementing num contacts CRM DB: " + output);
	}
}
//----------------------------------------------------------------------------------
void CRM::UpdateNotes(int ID, string notes)
{
	string querey = "UPDATE CRM SET notes = notes || \"\n" + DateTime::Now() + ":" + notes + "\n\" WHERE ID  " + to_string(ID);
	string output;

	if(!dbController.DoDBQuerey(querey,output))
	{
		DebugPrint( "error updateing notes CRM DB: " + output);
	}
}
//----------------------------------------------------------------------------------
bool CRM::AddNewContactEntry(CRM::ContactDetails &contact)
{
	string output;

	vector<DatabaseController::dbDataPair> insertData;
	insertData.push_back(make_pair("CustomerID", to_string(contact.customerID)));
	insertData.push_back(make_pair("Type", contact.type));
	insertData.push_back(make_pair("Successful", to_string(contact.successfulContact)));
	insertData.push_back(make_pair("notes", contact.notes));
	insertData.push_back(make_pair("Date",contact.date.ToString()));

	output.clear();

	if(!dbController.InsertNewDataEntry("ContactDetails",  insertData, output))
	{
		if(dbController.GetLastError() != "")
		{
			DebugPrint("error adding new value into CRM DB: " + dbController.GetLastError());
			return false;
		}
	}

	string querey = "UPDATE CRM SET NumContacts = NumContacts + 1,  WHERE ID = " + to_string(contact.customerID);
	if(!dbController.DoDBQuerey(querey,output))
	{
		DebugPrint( "error updating numc contacts entry in CRM DB: " + output);
		//return false;
	}
	return true;
}
//----------------------------------------------------------------------------------
CRM::ContactDetails CRM::GetContactEntry(int contactID)
{
	string output;

	vector<DatabaseController::dbDataPair> whereData;
	whereData.push_back(make_pair("ID", to_string(contactID)));

	if (!dbController.DoDBQuerey("ContactDetails", contactQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint("error getting customer contact from CRM DB: " + dbController.GetLastError());
		}
	}
	CRM::ContactDetails contact;
	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, contactQuereyList, dbResults);
	FillSingleContactDetailFromDBResults( dbResults,  contact);
	return contact;
}
//----------------------------------------------------------------------------------
vector<CRM::ContactDetails> CRM::GetAllCustomerContact(int customerID)
{
	string output;
	DatabaseController::dbDataPair whereData = make_pair("CustomerID", to_string(customerID));

	if (!dbController.DoDBQuerey("ContactDetails", contactQuereyList, whereData, output))
	{
		if (dbController.GetLastError() != "")
		{
			DebugPrint(" error getting baisc info: " + dbController.GetLastError());
		}
	}

	vector <DatabaseController::DBResult>  dbResults;
	DatabaseController::ParseDBOutput(output, contactQuereyList, dbResults);

	vector<CRM::ContactDetails> allContacts;
	for (int i = 0; i < dbResults.size(); i++)
	{
		CRM::ContactDetails contact;
		FillSingleContactDetailFromDBResults(dbResults, contact, i);
		allContacts.push_back(contact);
	}

	return 	allContacts;
}
//----------------------------------------------------------------------------------
void CRM::AddCustomerToTagDB(Customer customer)
{
	tags.AddNewItem(customer.name, customer.uniqueName, customer.id );
}
//----------------------------------------------------------------------------------
void CRM::AddTagToCustomer(int customer, string newtag)
{
	tags.TagItem(customer,newtag);
}
//----------------------------------------------------------------------------------
void CRM::AddTagsToCustomer(int customer, vector<string> &newtags)
{
	tags.TagItem(customer,newtags);
}
//----------------------------------------------------------------------------------
vector<string> CRM::GetAllCustomerTags(int customerID)
{
	return tags.GetAllTagsForItem(customerID);
}
//----------------------------------------------------------------------------------
vector<CRM::Customer> CRM::GetAllCustomersWithTag(string tag)
{
	vector<Customer> customers;
	vector<TaggingUtils::TaggedItem> items = tags.GetAllItemsWithTag(tag);
	for(size_t i = 0; i < items.size(); i++)
	{
		Customer temp = GetCustomerByID(items[i].id);
		customers.push_back(temp);
	}
	return customers;
}
//----------------------------------------------------------------------------------
vector<CRM::Customer> CRM::GetAllCustomersWithTheseTags(vector<string>  tagList)
{
	vector<Customer> customers;
	vector<TaggingUtils::TaggedItem> items = tags.GetAllItemsWithALLOfTheseTags(tagList);
	for(size_t i = 0; i < items.size(); i++)
	{
		Customer temp = GetCustomerByID(items[i].id);
		customers.push_back(temp);
	}
	return customers;
}