#pragma once
#include <string>
#include <vector>
#include "DatabaseController.h"
#include "DateTime.h"
using namespace std;
class CRM
{
private:
	DatabaseController dbController;
	bool verbose = true;
	vector<string> contactQuereyList;
public:
	struct ContactDetails
	{
		int id;
		int contactID;
		DateTime date;
		string type;//phone,email,fb message, etc
		bool successfulContact;
		string notes;
	};
	struct Customer
	{
		int id;
		string name;
		string phone;
		string email;
		string notes;
		string uniqueName;//used incase we dont have an email, like with facebook id contacts
		int lastContactID;
		bool active;
		int numContacts;
		DateTime lastContactDate;
		DateTime nextContactDate;
		DateTime dateAdded;
		Customer() { id = 0; }
		Customer(string n, string p,string e, string no)
		{
			name = n;
			phone = p;
			email = e;
			notes = no;
		}
	};
	CRM(string dbPath);

	void DebugPrint(string message);

	void AddNewEntryoCRM(string name, string email, string phone, string notes);
	void UpdateCustomerEntry(CRM::Customer & customer);
	//after each interaction with a customer, call this method
	void   UpdateContactAndNotes(int ID, string notes);
	vector<Customer> GetTodaysContacts();
	vector<Customer> GetAllActiveContacts();

	Customer  GetCustomerByEmail(string email);
	Customer  GetCustomerByPhone(string phone);
	Customer  GetCustomerByID(int id);
	
	void DeleteContact(Customer &customer);
	void DeleteContactByEmail(string email);
	
	
	void SetActiveStatus(int ID, int activeStatus);
	int GetActiveStatus(int ID);
	void DeactivateOldCustomers();
	void ExportAllData();
	void IncNumContacts(int ID);
	void UpdateNotes(int ID, string notes);

	void AddNewContactEntry(int customerID, string contactType, string notes, bool successful);

private:
	void FillSingleCustomerFromDBResults(vector <DatabaseController::DBResult> &dbResults, CRM::Customer &customer,int index);
};