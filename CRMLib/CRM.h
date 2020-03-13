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
	vector<string> customerQuereyList;
	vector<string> contactQuereyList;
public:
	struct ContactDetails
	{
		int id;
		int customerID;
		DateTime date;
		string type;//phone,email,fb message, etc
		bool successfulContact;
		string notes;
		void Clear()
		{
			id = 0;
			customerID = 0;
			type.clear();
			notes.clear();
			date = DateTime::Now();
		}
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
		void Clear()
		{
			id = 0;
			name.clear();
			phone.clear();
			email.clear();
			notes.clear();
			uniqueName.clear();
			lastContactID = 0;
			active = true;
			numContacts = 0;
			lastContactDate = DateTime::Now();
			nextContactDate = DateTime::Now();
			dateAdded = DateTime::Now();
		}
	};
	CRM(string dbPath);

	void DebugPrint(string message);

	bool AddNewCustomerEntry(CRM::Customer & customer);
	bool UpdateCustomerEntry(CRM::Customer & customer);
	//after each interaction with a customer, call this method
	void   UpdateContactAndNotes(int ID, string notes);
	vector<Customer> GetTodaysContacts();
	vector<Customer> GetAllActiveContacts();
	vector<CRM::Customer> GetOverdueContacts();

	Customer GetCustomerByEmail(string email);
	Customer GetCustomerByPhone(string phone);
	Customer GetCustomerByName(string name);
	Customer GetCustomerByID(int id);
	
	void DeleteContact(Customer &customer);
	void DeleteContactByEmail(string email);
	
	
	void SetActiveStatus(int ID, int activeStatus);
	int GetActiveStatus(int ID);
	void DeactivateOldCustomers();
	void ExportAllData();
	void IncNumContacts(int ID);
	void UpdateNotes(int ID, string notes);

	bool AddNewContactEntry(CRM::ContactDetails &contact);
	ContactDetails GetContactEntry(int contactID);
	vector<CRM::ContactDetails> GetAllCustomerContact(int customerID);
private:
	void FillSingleCustomerFromDBResults(vector <DatabaseController::DBResult> &dbResults, CRM::Customer &customer,int index);
	void FillSingleContactDetailFromDBResults(vector <DatabaseController::DBResult>& dbResults, CRM::ContactDetails& contact, int index);
};