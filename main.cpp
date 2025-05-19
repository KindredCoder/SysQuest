/*
* Developed by Kyle Gibson - AKA KindredCoder
* 
* Source Code is developed to be open sourced and free to share.
* This project and development is focued on continued learning and development of coding skills
* 
* 
* Contact: kyle.gibson2@snhu.edu
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits> // <-- Needed for numeric_limits
#include <ctime> // <-- Used for timestamps
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class Incident {
public:
	string system;
	string catagory;
	string issue;
	string resolution;
	string timestamp;
	string status;
};

// Status validation - Helper function
const vector<string> validStatuses = { "Open", "In Progress", "Resolved", "Escalated"};

bool isValidStatus(const string& status) {
	for (const auto& s : validStatuses) {
		if (s == status) {
			return true;
		}
	}
	return false;
}

// Catagory validation - Helper function
const vector<string> validCatagories = { "Hardware", "Software", "Network", "Security", "User Error", "Configuration", "Other" };

bool isValidCatagory(const string& catagory) {
	for (const auto& c : validCatagories) {
		if (c == catagory) {
			return true;
		}
	}
	return false;
}

// JSON serialization
void to_json(json& j, const Incident& i) {
	j = json{ 
			{"system", i.system}, 
			{"issue", i.issue}, 
			{"resolution", i.resolution}, 
			{"timestamp", i.timestamp},
			{"status", i.status},
			{"catagory", i.catagory}
	};
}

// JSON deserialization
void from_json(const json& j, Incident& i) {
	j.at("system").get_to(i.system);
	j.at("issue").get_to(i.issue);
	j.at("resolution").get_to(i.resolution);
	j.at("timestamp").get_to(i.timestamp);

	if (j.contains("status")) {
		j.at("status").get_to(i.status);
	}
	else {
		i.status = "Open"; // Default value if not present
	}

	if (j.contains("catagory")) {
		j.at("catagory").get_to(i.catagory);
	}
	else {
		i.catagory = "Other"; // Default value if not present
	}
}

// Current time function
string getCurrentTime() {
	time_t now = time(0);
	tm timeStruct;
	localtime_s(&timeStruct, &now); // Use localtime_s for thread safety

	char buf[80];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeStruct);
	return string(buf);
}

// Log a new incident
void logIncident() {
	Incident incident;

	cout << "Enter system name: ";
	getline(cin, incident.system);

	cout << "Describe the issue: ";
	getline(cin, incident.issue);

	// Catagory input with validation
	do {
		cout << "Enter Catagory (Hardware, Software, Network, Security, User Error, Configuration, Other): ";
		getline(cin, incident.catagory);

		if (incident.catagory.empty()) {
			incident.catagory = "Other"; // Default value
		}

		if (!isValidCatagory(incident.catagory)) {
			cout << "Invalid catagory. Please enter a valid catagory.\n";
			for (const auto& c : validCatagories) {
				cout << c << " ";
				cout << endl;
			}
		}
	} while (!isValidCatagory(incident.catagory));

	cout << "How was it resolved? ";
	getline(cin, incident.resolution);

	// Status input with validation
	do {
		cout << "Enter Status (Open, In Progress, Resolved, Escalated): ";
		getline(cin, incident.status);

		if (incident.status.empty()) {
			incident.status = "Open"; // Default value
		}

		if (!isValidStatus(incident.status)) {
			cout << "Invalid status. Please enter a valid status.\n";
			for (const auto& s : validStatuses) {
				cout << s << " ";
				cout << endl;
			}
		}
	} while (!isValidStatus(incident.status));

	// TimeStamp
	incident.timestamp = getCurrentTime();

	// Load existing JSON
	ifstream input("incident_log.json");
	json jList = json::array();
	if (input.is_open()) {
		input >> jList;
		input.close();
	}

	// Append new incident
	jList.push_back(incident);

	// Save back to file
	ofstream output("incident_log.json");
	output << jList.dump(4); // formatted JSON
	output.close();

	cout << "Incident logged to JSON successfully!\n";
}

// View all incidents
void viewIncidents() {
	ifstream input("incident_log.json");
	if (!input.is_open()) {
		cerr << "Failed to open JSON file.\n";
		return;
	}

	json jList;
	input >> jList;
	input.close();

	cout << "\n--- Incident Log ---\n";
	for (const auto& item : jList) {
		Incident i = item;
		cout << "System: " << i.system << endl;
		cout << "Issue: " << i.issue << endl;
		cout << "Catagory: " << i.catagory << endl;
		cout << "Resolution: " << i.resolution << endl;
		cout << "Status: " << i.status << endl;
		cout << "Timestamp: " << i.timestamp << endl;
		cout << "---\n";
	}
}

// Search by keyword
void searchIncidents() {
	ifstream input("incident_log.json");
	if (!input.is_open()) {
		cerr << "Failed to open JSON file.\n";
		return;
	}

	json jList;
	input >> jList;
	input.close();

	string keyword;
	cout << "Enter keyword to search: ";
	getline(cin, keyword);

	bool found = false;
	cout << "\n--- Search Results ---\n";
	for (const auto& item : jList) {
		Incident i = item;
		if (i.system.find(keyword) != string::npos ||
			i.issue.find(keyword) != string::npos ||
			i.resolution.find(keyword) != string::npos) {
			cout << "System: " << i.system << endl;
			cout << "Issue: " << i.issue << endl;
			cout << "Catagory: " << i.catagory << endl;
			cout << "Resolution: " << i.resolution << endl;
			cout << "Status: " << i.status << endl;
			cout << "Timestamp: " << i.timestamp << endl;
			cout << "---\n";
			found = true;
		}
	}

	if (!found) {
		cout << "No incidents found containing \"" << keyword << "\".\n";
	}
}

// Update records
void updateIncident() {
	// load JSON File
	ifstream input("incident_log.json");
	json jList = json::array();
	if (input.is_open()) {
		input >> jList;
		input.close();
	}

	// Display List with index numbers
	for (size_t i = 0; i < jList.size(); ++i) {
		Incident inc = jList[i];
		cout << "[" << i << "] "
			<< inc.system << " - " << inc.issue << endl;
	}

	// choose which record to edit
	int index;
	cout << "\nEnter the index of the incident to edit: ";
	cin >> index;
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear newline

	// Edit the selected record
	Incident incident = jList[index]; // from_json happens here automatically

	cout << "Current system: " << incident.system << "\nNew system (leave blank to keep): ";
	string userInput;
	getline(cin, userInput);
	if (!userInput.empty()) incident.system = userInput;

	cout << "Current issue: " << incident.issue << "\nNew issue (leave blank to keep): ";
	getline(cin, userInput);
	if (!userInput.empty()) incident.issue = userInput;

	cout << "Current catagory: " << incident.catagory << "\nNew catagory (leave blank to keep)\n[Hardware, Software, Network, Security, User Error, Configuration, Other]: ";
	getline(cin, userInput);
	if (!userInput.empty()) {
		if (isValidCatagory(userInput)) {
			incident.catagory = userInput;
		}
		else {
			cout << "Invalid catagory. Keeping exsisting catagory.\n";
		}
	}

	cout << "Current resolution: " << incident.resolution << "\nNew resolution (leave blank to keep): ";
	getline(cin, userInput);
	if (!userInput.empty()) incident.resolution = userInput;

	cout << "Current status: " << incident.status << "\nNew status (leave blank to keep)\n[Open, In Progress, Resolved, Escalated]: ";
	getline(cin, userInput);
	if (!userInput.empty()) {
		if (isValidStatus(userInput)) {
			incident.status = userInput;
		}
		else {
			cout << "Invalid status. Keeping exsisting status.\n";
		}
	}

	// Timestamp of modification
	incident.timestamp = getCurrentTime();

	// Reassign the updated object back to JSON
	jList[index] = incident; // trigger from_json()

	// Write back to JSON
	ofstream output("incident_log.json");
	output << jList.dump(4); // formatted JSON
	output.close();

	cout << "Incident updated successfully!\n";
}

// delete Incident records
void deleteIncident() {
	// load JSON File
	ifstream input("incident_log.json");
	json jList = json::array();
	if (input.is_open()) {
		input >> jList;
		input.close();
	}
	// Display List with index numbers
	for (size_t i = 0; i < jList.size(); ++i) {
		Incident inc = jList[i];
		cout << "[" << i << "] "
			<< inc.system << " - " << inc.issue << endl;
	}
	// choose which record to delete
	int index;
	cout << "\nEnter the index of the incident to delete: ";
	cin >> index;
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear newline
	
	// Optional: confirm deletion
	char confirm;
	cout << "Are you sure you want to delete this incident? (y/n): ";
	cin >> confirm;
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear newline

	if (confirm == 'y' || confirm == 'Y') {
		if (index >= 0 && index < jList.size()) {
			jList.erase(jList.begin() + index); // delete the incident

			// Write back to JSON
			ofstream output("incident_log.json");
			output << jList.dump(4); // formatted JSON
			output.close();

			cout << "Incident deleted successfully!\n";
		}
		else {
			cout << "Invalid index.\n";
		}
	} else {
			cout << "Deletion canceled.\n";
	
	}
}

// Filter by Status
void filterByStatus() {
	string query;
	cout << "Enter status to filter by (Open, In Progress, Resolved, Escalated): ";
	getline(cin, query);

	if (!isValidStatus(query)) {
		cout << "Invalid status. Please enter a valid status.\n";
		return;
	}

	ifstream input("incident_log.json");
	json jList;
	input >> jList;
	input.close();

	cout << "\n--- Incidents with status \"" << query << "\" ---\n";
	for (const auto& item : jList) {
		Incident i = item;
		if (i.status == query) {
			cout << "System: " << i.system << endl;
			cout << "Issue: " << i.issue << endl;
			cout << "Catagory: " << i.catagory << endl;
			cout << "Resolution: " << i.resolution << endl;
			cout << "Timestamp: " << i.timestamp << endl;
			cout << "---\n";
		}
	}
}

// Filter by Catagory
void filterByCatagory() {
	string query;
	cout << "Enter catagory to filter by \n(Hardware, Software, Network, Security, User Error, Configuration, Other): ";
	getline(cin, query);
	if (!isValidCatagory(query)) {
		cout << "Invalid catagory. Please enter a valid catagory.\n";
		return;
	}
	ifstream input("incident_log.json");
	json jList;
	input >> jList;
	input.close();
	cout << "\n--- Incidents with catagory \"" << query << "\" ---\n";
	for (const auto& item : jList) {
		Incident i = item;
		if (i.catagory == query) {
			cout << "System: " << i.system << endl;
			cout << "Issue: " << i.issue << endl;;
			cout << "Resolution: " << i.resolution << endl;
			cout << "Status: " << i.status << endl;
			cout << "Timestamp: " << i.timestamp << endl;
			cout << "---\n";
		}
	}
}

// Exporting to CSV
void exportToCSV(const vector<Incident>& results, const string& filename = "filtered_incidents.csv") {
	ofstream file(filename);
	if (!file.is_open()) {
		cerr << "Could not open file for writing.\n";
		return;
	}

	// Write CSV header
	file << "System,Issue,Catagory,Resolution,Status,Timestamp\n";

	// Write each incident
	for (const auto& i : results) {
		file << '"' << i.system << "\",\""
			<< i.issue << "\",\""
			<< i.catagory << "\",\""
			<< i.resolution << "\",\""
			<< i.status << "\",\""
			<< i.timestamp << "\"\n";
	}

	file.close();
	cout << "Filtered incidents exported to " << filename << " successfully!\n";
}

// Filter Incidents by multiple options
void filterIncidents() {
	string statusInput, catagoryInput;

	cout << "Enter status of filter by (or leave blank): ";
	getline(cin, statusInput);

	if (!statusInput.empty() && !isValidStatus(statusInput)) {
		cout << "Invalid status. Available options are: ";
		for (const auto& s : validStatuses) cout << s << " ";
		cout << endl;
		return;
	}

	cout << "Enter catagory to filter by (or leave blank): ";
	getline(cin, catagoryInput);

	if (!catagoryInput.empty() && !isValidCatagory(catagoryInput)) {
		cout << "Invalid catagory. Available options are: ";
		for (const auto& c : validCatagories) cout << c << " ";
		cout << endl;
		return;
	}

	// Load incidents
	ifstream input("incident_log.json");
	if (!input.is_open()) {
		cerr << "Could not open incident log.\n";
		return;
	}

	json jList;
	input >> jList;
	input.close();

	cout << "\n--- Filtered Incidents ---\n";
	bool found = false;

	vector<Incident> filteredResults;

	for (const auto& item : jList) {
		Incident i = item;

		bool statusMatch = statusInput.empty() || i.status == statusInput;
		bool catagoryMatch = catagoryInput.empty() || i.catagory == catagoryInput;

		if (statusMatch && catagoryMatch) {
			// Display
			cout << "System: " << i.system << endl;
			cout << "Issue: " << i.issue << endl;
			cout << "Catagory: " << i.catagory << endl;
			cout << "Resolution: " << i.resolution << endl;
			cout << "Status: " << i.status << endl;
			cout << "Timestamp: " << i.timestamp << endl;
			cout << "---\n";
			

			// Store for CSV export
			filteredResults.push_back(i);
		}
	}

	if (filteredResults.empty()) {
		cout << "No incidents found matching the criteria.\n";
	} else {
		string choice;
		cout << "Would you like to export these to CSV? (y/n): ";
		getline(cin, choice);
		if (choice == "y" || choice == "Y") {
			exportToCSV(filteredResults);
		}	
	}
}


// Main menu
int main() {
	int choice;

	while (true) {
		cout << "\nSysQuest: Incident Codex\n";
		cout << "1. Log new incident\n";
		cout << "2. View all incidents\n";
		cout << "3. Search incidents by keyword\n";
		cout << "4. Update incident\n";
		cout << "5. Delete incident\n";
		cout << "6. Filter incidents by status\n";
		cout << "7. Filter incidents by catagory\n";
		cout << "8. Filter incidents by multiple options\n";
		cout << "9. Exit\n";
		cout << "Choose an option: ";

		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear newline

		switch (choice) {
		case 1:
			logIncident();
			break;
		case 2:
			viewIncidents();
			break;
		case 3:
			searchIncidents();
			break;
		case 4:
			updateIncident();
			break;
		case 5:
			deleteIncident();
			break;
		case 6:
			filterByStatus();
			break;
		case 7:
			filterByCatagory();
			break;
		case 8:
			filterIncidents();
			break;
		case 9:
			cout << "Exiting program. Farewell, sys-knight.\n";
			return 0;
		default:
			cout << "Invalid option. Try again\n";
		}
	}

	return 0;
}