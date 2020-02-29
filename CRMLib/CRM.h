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
	struct Contact
	{
		int id;
		string name;
		string phone;
		string email;
		string notes;
		bool active;
		int numContacts;
		DateTime lastContactDate;
		DateTime nextContactDate;
		DateTime dateAdded;
		Contact() {}
		Contact(string n, string p,string e, string no)
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
	void UpdateCustomerEntry(CRM::Contact & contact);
	//after each interaction with a customer, call this method
	void   UpdateContactAndNotes(int ID, string notes);
	vector<Contact> GetTodaysContacts();
	vector<Contact> GetAllActiveContacts();

	Contact  GetContactByEmail(string email);
	Contact  GetContactByPhone(string phone);
	Contact  GetContactByID(int id);
	
	void DeleteContact(Contact &contact);
	void DeleteContactByEmail(string email);
	
	
	void SetActiveStatus(int ID, int activeStatus);
	int GetActiveStatus(int ID);
	void DeactivateOldCustomers();
	void ExportAllData();
	void IncNumContacts(int ID);
	void UpdateNotes(int ID, string notes);
private:
	void FillSingleCatactFromDBResults(vector <DatabaseController::DBResult> &dbResults, CRM::Contact &contact,int index);
};