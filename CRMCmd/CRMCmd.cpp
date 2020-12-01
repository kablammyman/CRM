// CRMCmd.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "CommandLineUtils.h"
#include "ArgParser.h"
#include "DateTime.h"

#include "CRMBackEnd.h"
#include "StringUtils.h"
#include "CSVHandler.h"
#include "FileUtils.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

void FixHangonsBrokenCSVs(string path)
{
	vector<string> allFiles = FileUtils::GetAllFileNamesInDir(path, "csv", true);
	//string testFile = "C:\\Users\\kabla\\Desktop\\hangouts convos\\(214) 998-3204.csv";
	//string newFile = "C:\\Users\\kabla\\Desktop\\hangouts convos\\(214) 998-3204.txt";
	string marker = "2020,";//if you see this, then its a valid csv entry, otehrwise, we have a continuation of prev line
	for (size_t i = 0; i < allFiles.size(); i++)
	{
		string line;
		ifstream curCSV(allFiles[i]);
		string newFileName = allFiles[i] + ".txt";
		ofstream newCSV(newFileName);
		vector <string>curFileData;
		//put file contents in vector, since we will need random access
		while (getline(curCSV, line))
		{
			curFileData.push_back(line);
		}
		size_t j = 0, convoTextStart;
		bool insideQuote = false;
		string newLine;
		while(j < curFileData.size())
		{
			size_t foundCurLine = curFileData[j].find(marker);
			//we are at the start of a valid csv entry
			if (foundCurLine != string::npos)
			{
				if (!insideQuote)
				{
					insideQuote = true;
					convoTextStart = foundCurLine + marker.size();
					//insert the quotes at the start of the "mssage" col"
					newLine = curFileData[j].substr(0, convoTextStart);
					newLine += "\"" + curFileData[j].substr(convoTextStart);
					j++;
				}
				else
				{
					//cap it off with the closing quote and new line
					newLine += "\"\n";
					newCSV << newLine;
					newLine.clear();
					insideQuote = false;
				}
			}
			else
			{
				//the '~' is a stand in for the nwe line...im not sure if i want actual new lines or not, but if i do, it will be easy to do a find and repalce on that char
				newLine += "~"+curFileData[j];
				j++;
			}
		}
		if (!newLine.empty())
		{
			newLine += "\"\n";
			newCSV << newLine;
		}
	}
}
void ImportFromHangonsOutput(CRMBackEnd &cbe, string path)
{
	//hang ons is an open source script that takes gogle hangouts json and gives you a few options
	//i used the create csvs option where it make sa csv for each phone number\contact name
	//headers go like so (they are not named in teh file, so first entry is actual data):
	vector<string> headers = {"name","smsTime","smsDayAndDate","message"};

	vector<string> allFiles = FileUtils::GetAllFileNamesInDir(path, "txt", true);

	for (size_t i = 0; i < allFiles.size(); i++)
	{
		cout << allFiles[i] << endl;
		string phoneNumber = FileUtils::GetFileNameNoExt(allFiles[i]);
		phoneNumber.erase(phoneNumber.length() - 4);
		cbe.DoPhoneLookup(phoneNumber);

		if (cbe.curCustomer.phone.empty())
		{
			cbe.curCustomer.phone = phoneNumber;
			cbe.curCustomer.uniqueName = phoneNumber;
		}

		//each unique date is a contact, so all teh messages witht he same date needs to be grouped togeher
		//new dates means a new contactdetail obj
		vector<CRM::ContactDetails> convos;
		CRM::ContactDetails curConvo;
		curConvo.date.year = 1900;
		curConvo.successfulContact = true;

		
		vector<string> foundTags;
		CSVHandler curCSV;
		curCSV.CreateCSVHeader(headers);
		curCSV.ReadCSVFile(allFiles[i], ',',false);
		for (size_t j = 0; j < curCSV.GetCSVLength(); j++)
		{
			string messageSender = curCSV.GetColValue("name", j);
			string messageTime = curCSV.GetColValue("smsTime", j);
			DateTime date;
			string messageDate = curCSV.GetColValue("smsDayAndDate", j);
			vector<string> dateParts = StringUtils::Tokenize(messageDate, ' ');
			date.year = StringUtils::GetIntFromString(dateParts[3]);
			date.SetMonthFromAbrv(dateParts[1]);
			date.day = StringUtils::GetIntFromString(dateParts[2]);
			string message = curCSV.GetColValue("message", j);
			
			//sometimes the name is embeded in the mssage, i need a way to find them
			
			if (cbe.curCustomer.name.empty())
				cbe.curCustomer.name = cbe.FindNameInMessage(message);
			
			if (messageSender.find("Victor") == string::npos && messageSender.find("2340998") == string::npos)
			if (cbe.curCustomer.email.empty())
				cbe.curCustomer.email = cbe.FindEmailInMessage(message);
			cbe.FindTagsInText(message,foundTags);

			if (curConvo.date != date)
			{
				if (curConvo.date.year > 1900)
				{
					//save this convo and create a new one
					convos.push_back(curConvo);
					curConvo.Clear();
				}
				curConvo.date = date;
			}
			else
			{
				curConvo.notes += messageSender+ "("+messageTime+"): " + message + "\n";
			}
		}
		cbe.AddTags(foundTags);
		//after fidning all the data needed, lets save it
		cbe.SaveCurrentInfo();
		cbe.AddConvosToCurrentCustomer(convos);
		
	}
}

using namespace std;
int main(int argc, const char *argv[])
{
	cout << "Land CRM CMD\n";
	cout << " Run date: " << DateTime::PrettyPrintNow() <<endl;

	ArgParser args;
	args.AddArg("test", ArgParser::FLAG, "-x", "--test", "test differnt functions and stuff");
	args.AddArg("export", ArgParser::PATH, "-e", "--exportCSV", "csv of custmers based on search criteria");
	args.AddArg("import", ArgParser::PATH, "-i", "--importCSV", "add in new customers (or update existing ones)");
	args.AddArg("tags", ArgParser::STORE, "-t", "--tags", "specfiy what tags we want to search");
	args.AddArg("name", ArgParser::STORE, "-n", "--name", "get the info of a customer from thier name");
	args.AddArg("phone", ArgParser::STORE, "-p", "--phone", "get the info of a customer from thier name");
	args.AddArg("email", ArgParser::STORE, "-m", "--email", "get the info of a customer from thier email");
	
	if (!args.ParseArgs(argc, argv))
	{
		cout << args.HelpText() << endl;
		exit(0);
	}
	
	CRMBackEnd cbe;
	cbe.SetDB("\\\\SERVER\\documents\\land investing\\Database\\CRM.db");

	if (args.IsArgEnabled("test"))
	{
		cout << cbe.FindEmailInMessage("Patricio.osorio400@gmail.com") << endl;
		cout << cbe.FindEmailInMessage("email: Patricio.osorio400@gmail.com") << endl;
		cout << cbe.FindEmailInMessage("email: Patricio.osorio400@gmail.com is that okay?") << endl;
		cout << cbe.FindEmailInMessage("Okay, I'll create the docs in a few hours when I'm back in front of my computer. You can zelle me at victor@victorsvacantland.com") << endl;
		string message = "its 2500 obo, you can check out the details here:\nhttps://victorsvacantland.com/parcels/Mobile/29_02_44_0_007_201.XXX/Mobile_parcel_29_02_44_0_007_201.XXX.php";
		vector<string> foundTags;
		cbe.FindTagsInText(message,  foundTags);
		
		exit(0);
	}

	else if (args.IsArgEnabled("export"))
	{

	}
	else if (args.IsArgEnabled("import"))
	{
		//FixHangonsBrokenCSVs(args.GetArgValue("import"));
		ImportFromHangonsOutput(cbe,args.GetArgValue("import"));
	}
	else if (args.IsArgEnabled("tags"))
	{
		string tags = args.GetArgValue("tags");
		vector<CRM::Customer> customerList;
		
		cbe.GetAllCustersThatHasAllTheseTag(tags,customerList);

		for (size_t i = 0; i < customerList.size(); i++)
		{
			cout << "Name: " << customerList[i].name <<endl;
			cout << "Phone: " << customerList[i].phone <<endl;
			cout << "Email: " << customerList[i].email <<endl;
			cout << "Notes: " << customerList[i].notes <<endl;
		}
	}
	cout << "done!\n";
}