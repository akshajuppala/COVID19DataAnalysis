/*main.cpp*/

//
// Akshaj Uppala - 665138061
// 
// This project is intended to keep us informed of the statistics of the corona virus 
// around the world using the daily reports that we receive daily from the countries. It has different commands 
// that helps it show the statistics in different perspectives. It can show the top 10 countries that are affected and show the 
// breakdown of the disease spread to every country.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <experimental/filesystem>
#include <locale>
#include <iomanip>
#include <vector>
#include <map>

using namespace std;
namespace fs = std::experimental::filesystem;

//struct for the corona cases in each country or for storing the latest information
struct COVIDcases {
	int confirmed;
	int deaths;
	int recovered;
};

//the struct which stores the data about a particular country
struct countryData{

	struct COVIDcases latest;
	map<string,COVIDcases> history;
};

//The function required for inputting the daily report one after the other
void inputDailyReport(map<string,struct countryData>& info,int& numCountries,string file){
	
	ifstream infile;
	string line = "";
	
	infile.open(file);
		if(!infile.is_open()){
			cout << "Could not open the file "<< file << endl;
		    return;
		}
		
		getline(infile,line);
		while(getline(infile,line)){
			if(line[0] == '"'){
				line.erase(0,1);
				size_t pos = line.find(',');
				line.erase(pos,1);
				pos = line.find('"');
				line.erase(pos,1);
			}
			
			stringstream s(line);
			string province,country,last_update;
			string confirmed, deaths, recovered;
			int confirmedInt, deathsInt, recoveredInt;
			
			getline(s,province,',');
			getline(s,country,',');
			getline(s,last_update,',');
			getline(s,confirmed,',');
			getline(s,deaths,',');
			getline(s,recovered,',');
			
			if(confirmed == "")
				confirmed = "0";
			if(deaths == "")
				deaths = "0";
			if(recovered == "")
				recovered = "0";
			confirmedInt = stoi(confirmed);
			deathsInt = stoi(deaths);
			recoveredInt = stoi(recovered);
			
			if(country == "Mainland China")
				country = "China";
			
			
			struct COVIDcases temp;
			
			//branch for the countries being inputted the first time
			if(info.count(country) == 0){
				struct countryData tempData;
				
				temp.confirmed = confirmedInt;
				temp.deaths = deathsInt;
				temp.recovered = recoveredInt;
				tempData.latest.confirmed = 0;
				tempData.latest.deaths = 0;
				tempData.latest.recovered = 0;
				tempData.history.emplace(file,temp);
				info.emplace(country,tempData);
				numCountries++;
			}
			//branch for the countries in the data structure but which have not been updated with the latest date information
			else if(info.count(country) == 1 && info.at(country).history.count(file) == 0){

				temp.confirmed = confirmedInt;
				temp.deaths = deathsInt;
				temp.recovered = recoveredInt;
				info.at(country).history.emplace(file, temp);
			} 
			//branch for inputting the country information for different provinces of the country
			else {
				
				temp = info.at(country).history.at(file);
				temp.confirmed += confirmedInt;
			    temp.deaths += deathsInt;
				temp.recovered += recoveredInt;
				info.at(country).history.at(file) = temp;
			}
			
			
		}
		line = "";
		infile.close();
}

//The function called for making the data structure
map<string, struct countryData> makeTheDataStructure(vector<string> files, COVIDcases& total, string& latestDate){
	
	size_t counting = 0;
	int numCountries = 0;
	map<string, struct countryData> info;

	//calling the function N times where N is the number of daily reports
	while(counting < files.size()){
		
		inputDailyReport(info, numCountries, files.at(counting));
		if(files.at(counting) > latestDate){
			latestDate = files.at(counting);
		}
		counting++;

	}

	cout << ">> Processed " << counting << " daily reports" << endl;
	cout << ">> Processed 2 files of world facts" << endl;
	cout << ">> Current data on " << numCountries << " countries" << endl;
	cout << endl;
	
	//for loop loops through every countrie's latest date to acquire the latest Data information
	// and calculates the total in the process

	for(map<string, struct countryData>::iterator iter = info.begin(); iter != info.end(); ++iter){
		COVIDcases latestData;
		if(iter->second.history.count(latestDate) == 0){
			
			continue;
		}
		latestData = iter->second.history.at(latestDate);
		iter->second.latest = latestData;
		total.confirmed += latestData.confirmed;
		total.deaths += latestData.deaths;
		total.recovered += latestData.recovered;
	}
	
	latestDate = latestDate.substr(16, latestDate.find(".csv")-16);
	return info;
}

//function shows the commands that are available
void doHelp(){
	cout << "Available commands:" << endl;
	cout << " <name>: enter a country name such as US or China" << endl;
	cout << " countries: list all countries and most recent report" << endl;
	cout << " top10: list of top 10 countries based on most recent # of confirmed cases" << endl;
	cout << " totals: world-wide totals of confirmed, deaths, recovered" << endl;
	cout << " worldfacts: list of all countries and their population and life-expectancies" << endl;
	cout << endl;
}

//function for showing the stats of every country by looping through the complete data structure using iterator
void doShowAllCountryStats(map<string, struct countryData> info){
	struct COVIDcases current;
	for(map<string, struct countryData>::iterator iter = info.begin(); iter != info.end(); ++iter){
		current = iter->second.latest;
		cout << iter->first << ": " << current.confirmed << ", " << current.deaths << ", " << current.recovered << endl;
	}
}

//function displays the top10 countries 
void doShowTop10(map<string, struct countryData> info){
	
	map<int, string> topCases;
	//keeping all the countries in the map with their confirmed cases as keys which 
	//arranges them in ascending order when we are accessing the countries
	for(map<string, struct countryData>::iterator it = info.begin(); it!=info.end(); ++it){
		
		topCases.emplace(it->second.latest.confirmed, it->first);
	}
	
	//iter1 is 10 countries ahead of iter2
	map<int, string>::iterator iter1 = topCases.begin();
	map<int, string>::iterator iter2 = topCases.begin();
	for(int i = 0; i < 10; ++i){
		iter1++;
	}
	//iter1 checks the map and stops the iter2 when it is 10 countries before the end
	while(iter1!= topCases.end()){
		iter1++;
		iter2++;
	}
	
	//the last ten countries of the map kept in the vector
	vector<string> topTen;
	for(int i = 1; i <=10; ++i){
		topTen.push_back(iter2->second);
		iter2++; 
	}
	//outputting the countries in their order
	for(int i = 9; i >=0; i--){
		cout << 10-i << ". " << topTen[i] << ": " << info.at(topTen[i]).latest.confirmed << endl;
	}
}

//function to show total confirmed, recovered cases and deaths
void doTotals(struct COVIDcases totals, string latestDate){
	
	cout << "As of " << latestDate << ", the world-wide totals are:" << endl;
	cout << " confirmed: " << totals.confirmed << endl;
	cout << " deaths: " << totals.deaths << " (" << (totals.deaths*100.0)/totals.confirmed << "%)" << endl;
	cout << " recovered: " << totals.recovered << " (" << (totals.recovered*100.0)/totals.confirmed << "%)" << endl;
}

//outputting everyline of the timeline that should be outputted
void doOutput(map<string, COVIDcases>::iterator iter2,string command,int& actualCount,bool flag, bool flag2, int count){
	//flags are created to allow the output of information only when required
	//actualCount is incremented
	if(command == "c" && flag == false && (iter2->second.confirmed != 0 || flag2 == true)){
		cout << iter2->first.substr(16,iter2->first.find(".csv")-16) << " (day " << count << "): " << iter2->second.confirmed << endl;
		actualCount++;
	}
	else if(command == "d" && flag == false && (iter2->second.deaths != 0 || flag2 == true)){
		cout << iter2->first.substr(16,iter2->first.find(".csv")-16) << " (day " << count << "): " << iter2->second.deaths << endl;
		actualCount++;
	}
	else if(command == "r" && flag == false && (iter2->second.recovered != 0 || flag2 == true)){
		cout << iter2->first.substr(16,iter2->first.find(".csv")-16) << " (day " << count << "): " << iter2->second.recovered << endl;
		actualCount++;
	}
}

//funtion to create output about the kind of command that is entered
bool doBranchedOutput(string command){
	if(command == "c"){
		cout << " Confirmed:" << endl;
		return false;
	}
	else if(command == "r"){
		cout << " Recovered:" << endl;
		return false;
	}
	else if(command == "d"){
		cout << " Deaths:" << endl;
		return false;
	}
	else{
		cout << endl;
		return true;
	}
}

//function for showing the individual statistics of each country
void doShowCountryStats(map<string, struct countryData> info, string country, map<string,int> populations, map<string,double> life_expectancies){
	
	
	cout << " Population: " << populations.at(country) << endl;
	cout << "Life Expectancy: " << life_expectancies.at(country) << " years" << endl;
	cout << "Latest data:" << endl;
	cout << " confirmed: " << info.at(country).latest.confirmed << endl;
	cout << " deaths: " << info.at(country).latest.deaths << endl;
	cout << " recovered: " << info.at(country).latest.recovered << endl;
	
	int a = 0;     //created to estrict the entry into the if branch of first confirmed cases after it has entered once
	int b = 0;     //created for outputting none in case the recorded deaths are 0
	for(map<string, COVIDcases>::iterator it = info.at(country).history.begin(); it!=info.at(country).history.end(); ++it){
		
		if(it->second.confirmed > 0 && a ==0){
			cout << "First confirmed case: " << it->first.substr(16, it->first.find(".csv")-16) << endl;
			a = 1;
		}
		
		if(it->second.deaths > 0){
			cout << "First recorded death: " << it->first.substr(16, it->first.find(".csv")-16) << endl;
			b = 1;
			break;
		}
	}
	if(a != 1){
		cout << "First confirmed case: none" << endl;
	}
	if(b != 1){
		cout << "First recorded death: none" << endl;
	}
	
	cout << "Do you want to see a timeline? Enter c/d/r/n>";
	string command;
	int count = 1;          //required for counting all the dates even before the first confirmed case
	int actualCount = 1;    //created for counting the days since the first case of death or recovery or confirmation
	bool flag = false;      //flag initially allowing the print of statements until 7 statements are printed and then turned off
	bool flag2 = false;     //flag required for the last 7 print statements where it fine even if the cases are zero,
	                        //hence "or" is used there
	
	getline(cin, command);
	
	map<string, COVIDcases>::iterator iter1 = info.at(country).history.begin();
	map<string, COVIDcases>::iterator iter2 = info.at(country).history.begin();
	
	for(int i = 0; i < 7; ++i){
		iter1++;
		if(iter1 == info.at(country).history.end()){
			break;
		}
	}
	
	if(doBranchedOutput(command)){
		return;
	}
		
	while(iter2 != info.at(country).history.end()){
		
		if(actualCount == 8 && iter1 != info.at(country).history.end()){
			cout << " .\n .\n ." << endl;
			flag = true;
			actualCount = 0;
		}
		if(iter1 == info.at(country).history.end() && flag == true){
			flag = false;
			flag2 = true;
		}
		
		doOutput(iter2, command,actualCount, flag, flag2, count);
		
		iter1++;
		iter2++;
		count++;
	}
}


//function of my choice which shows the population and life expectancy of every country
void doWorldFacts(map<string,int> populations, map<string,double> life_expectancies){
	
	cout << "Countries: population, life_expectancy" << endl;
	for(map<string, int>::iterator iter = populations.begin(); iter != populations.end(); ++iter){
		if(life_expectancies.count(iter->first) == 1)
			cout << iter->first << ": " << iter->second << "; " << life_expectancies.at(iter->first) << endl;
		else
			cout << iter->first << ": " << iter->second << "; -"<< endl;
	}
}
//
// getFilesWithinFolder
//
// Given the path to a folder, e.g. "./daily_reports/", returns 
// a vector containing the full pathnames of all regular files
// in this folder.  If the folder is empty, the vector is empty.
//
vector<string> getFilesWithinFolder(string folderPath)
{
    vector<string> files;

    for (const auto& entry : fs::directory_iterator(folderPath))
    {
      files.push_back(entry.path().string());
    }

    return files;
}


//
// main:
//
int main()
{
    cout << "** COVID-19 Data Analysis **" << endl;
    cout << endl;
    cout << "Based on data made available by John Hopkins University" << endl;
    cout << "https://github.com/CSSEGISandData/COVID-19" << endl;
    cout << endl;

    //
    // setup cout to use thousands separator, and 2 decimal places:
    //
    cout.imbue(std::locale(""));
    cout << std::fixed;
    cout << std::setprecision(2);

    //
    // get a vector of the daily reports, one filename for each:
    //
    vector<string> files = getFilesWithinFolder("./daily_reports/");
  
    //
    // TODO:
    //
	
	ifstream inlife;
	ifstream inpopulations;
	string command = "";
	map<string, struct countryData> info;
	string line;
	string latestDate = "";
	string position,country,value = "";
	
	//opening the file life_expectancies.csv ----------
	inlife.open("./worldfacts/life_expectancies.csv");
	if(!inlife.is_open()){
		cout << "Could not open the file life_expectancies.csv" << endl;
		return 1;
	}
	
	map<string,double> life_expectancies;
	double valuelife = 0.0;
	
	getline(inlife,line);
	while(getline(inlife,line)){
		stringstream l(line);
		getline(l, position, ',');
		getline(l, country, ',');
		getline(l, value);
		valuelife = stod(value); 
		life_expectancies.emplace(country,valuelife);
	}
	inlife.close();
	
	//opening the file populations.csv ----------
	inpopulations.open("./worldfacts/populations.csv");
	if(!inpopulations.is_open()){
		cout << "Could not open the file population.csv" << endl;
		return 1;
	}
	
	map<string,int> populations;
	int valuepopulation = 0;
	
	getline(inpopulations,line);
	while(getline(inpopulations,line)){
		stringstream p(line);
		getline(p, position, ',');
		getline(p, country, ',');
		getline(p, value);
		valuepopulation = stoi(value);
		populations.emplace(country,valuepopulation);
	}
	inpopulations.close();
	
	struct COVIDcases total;
	total.confirmed = 0;
	total.deaths = 0;
	total.recovered = 0;
	
	
 	info = makeTheDataStructure(files, total, latestDate);
	
	cout << "Enter command (help for list, # to quit)> ";
	getline(cin,command);
	
	//while loop to take in commands
	while(command != "#"){
		
		if(command == "help"){
			doHelp();
		}
		else if(command == "countries"){
			doShowAllCountryStats(info);
		}
		else if(command == "top10"){
			doShowTop10(info);
		}
		else if(command == "totals"){
			doTotals(total, latestDate);
		}
		else if(command == "worldfacts"){
			doWorldFacts(populations, life_expectancies);
		}
		else{
			if(info.count(command) == 0)
				cout << "country or command not found..." << endl;
			else
				doShowCountryStats(info,command, populations, life_expectancies);
		}
		cout << "Enter command> ";
		getline(cin, command);
	}
	
    return 0;
}
