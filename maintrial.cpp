/*main.cpp*/

//
// Akshaj Uppala - 665138061
// 
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

struct COVIDcases {
	int confirmed;
	int deaths;
	int recovered;
};

struct countryData{

	struct COVIDcases latest;
	map<string,COVIDcases> history;
};

map<string, struct countryData> makeTheDataStructure(vector<string> files, COVIDcases& total, string& latestDate){
	
	size_t counting = 0;
	map<string, struct countryData> info;
	ifstream infile;
	string line = "";
	int numCountries = 0;
	
	while(counting < files.size()){
		
		infile.open(files.at(counting));
		if(!infile.is_open()){
			cout << "Could not open the file "<< files.at(counting) << endl;
		    return info;
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
			
			if(info.count(country) == 0){
				struct countryData tempData;
				
				temp.confirmed = confirmedInt;
				temp.deaths = deathsInt;
				temp.recovered = recoveredInt;
				tempData.latest.confirmed = 0;
				tempData.latest.deaths = 0;
				tempData.latest.recovered = 0;
				tempData.history.emplace(files.at(counting),temp);
				info.emplace(country,tempData);
				numCountries++;
			} else if(info.count(country) == 1 && info.at(country).history.count(files.at(counting)) == 0){

				temp.confirmed = confirmedInt;
				temp.deaths = deathsInt;
				temp.recovered = recoveredInt;
				info.at(country).history.emplace(files.at(counting), temp);
			} else {
				
				temp = info.at(country).history.at(files.at(counting));
				temp.confirmed += confirmedInt;
			    temp.deaths += deathsInt;
				temp.recovered += recoveredInt;
				info.at(country).history.at(files.at(counting)) = temp;
			}
			
			if(files.at(counting) > latestDate){
				latestDate = files.at(counting);
			}
		}
		line = "";
		counting++;
		infile.close();
	}

	cout << ">> Processed " << counting << " daily reports" << endl;
	cout << ">> Processed 2 files of world facts" << endl;
	cout << ">> Current data on " << numCountries << " countries" << endl;
	cout << endl;
	
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

void doHelp(){
	cout << "Available commands:" << endl;
	cout << " <name>: enter a country name such as US or China" << endl;
	cout << " countries: list all countries and most recent report" << endl;
	cout << " top10: list of top 10 countries based on most recent # of confirmed cases" << endl;
	cout << " totals: world-wide totals of confirmed, deaths, recovered" << endl;
	cout << endl;
}

void doShowAllCountryStats(map<string, struct countryData> info){
	struct COVIDcases current;
	for(map<string, struct countryData>::iterator iter = info.begin(); iter != info.end(); ++iter){
		current = iter->second.latest;
		cout << iter->first << ": " << current.confirmed << ", " << current.deaths << ", " << current.recovered << endl;
	}
}

void doShowTop10(map<string, struct countryData> info){
	
	map<int, string> topCases;
	
	for(map<string, struct countryData>::iterator it = info.begin(); it!=info.end(); ++it){
		
		topCases.emplace(it->second.latest.confirmed, it->first);
	}
	
	map<int, string>::iterator iter1 = topCases.begin();
	map<int, string>::iterator iter2 = topCases.begin();
	for(int i = 0; i < 10; ++i){
		iter1++;
	}
	while(iter1!= topCases.end()){
		iter1++;
		iter2++;
	}
	vector<string> topTen;
	for(int i = 1; i <=10; ++i){
		topTen.push_back(iter2->second);
		iter2++; 
	}
	for(int i = 9; i >=0; i--){
		cout << 10-i << ". " << topTen[i] << ": " << info.at(topTen[i]).latest.confirmed << endl;
	}
}

void doTotals(struct COVIDcases totals, string latestDate){
	
	cout << "As of " << latestDate << ", the world-wide totals are:" << endl;
	cout << " confirmed: " << totals.confirmed << endl;
	cout << " deaths: " << totals.deaths << " (" << (totals.deaths*100.0)/totals.confirmed << "%)" << endl;
	cout << " recovered: " << totals.recovered << " (" << (totals.recovered*100.0)/totals.confirmed << "%)" << endl;
}

bool doInitialSetup(map<string, COVIDcases>::iterator& iter1, map<string, COVIDcases>::iterator& iter2,
					map<string, struct countryData> info, string command, string country, string firstConfirmedDate,
					string firstDeathDate, string firstRecoveredDate, int count, int dayCount)
{

	for(int i = 0; i < 7; ++i){
		iter1++;
		if(iter1 == info.at(country).history.end()){
			break;
		}
	}
	
	if(command == "c"){
		cout << " Confirmed:" << endl;
		if(firstConfirmedDate == "")
			return true;
		while(iter2->first != firstConfirmedDate){
			iter2++; iter1++; dayCount++;
		}
		return false;
	}
	else if(command == "r"){
		cout << " Recovered:" << endl;
		if(firstRecoveredDate == "")
			return true;
		while(iter2->first != firstRecoveredDate){
 			iter2++; iter1++; dayCount++;
		}
		return false;
	}
	else if(command == "d"){
		cout << " Deaths:" << endl;
		if(firstDeathDate == "")
			return true;
		while(iter2->first != firstDeathDate){
			iter2++; iter1++; dayCount++;
		}
		return false;
	}
	else{
		cout << endl;
		return true;
	}
	
}

void doShowCountryStats(map<string, struct countryData> info, string country, map<string,int> populations, map<string,double> life_expectancies){
	//cout << endl;
	cout << " Population: " << populations.at(country) << endl;
	cout << "Life Expectancy: " << life_expectancies.at(country) << " years" << endl;
	cout << "Latest data:" << endl;
	cout << " confirmed: " << info.at(country).latest.confirmed << endl;
	cout << " deaths: " << info.at(country).latest.deaths << endl;
	cout << " recovered: " << info.at(country).latest.recovered << endl;
	
	int a, b, c = 0;
	string firstConfirmedDate,firstDeathDate,firstRecoveredDate  = "";
	for(map<string, COVIDcases>::iterator it = info.at(country).history.begin(); it!=info.at(country).history.end(); ++it){
		
		if(it->second.confirmed > 0 && a ==0){
			cout << "First confirmed case: " << it->first.substr(16, it->first.find(".csv")-16) << endl;
			firstConfirmedDate = it->first;
			a = 1;
		}
		
		if(it->second.recovered > 0 && c == 0){
			firstRecoveredDate = it->first;
			c = 1;
		}
		
		if(it->second.deaths > 0 && b == 0){
			cout << "First recorded death: " << it->first.substr(16, it->first.find(".csv")-16) << endl;
			firstDeathDate = it->first;
			b = 1;	
		}
		if(a==1 && b == 1 && c ==1){
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
	getline(cin, command);
	int dayCount = 1;
	int count = 1;
	bool flag = false;
	
	
	map<string, COVIDcases>::iterator iter1, iter2 = info.at(country).history.begin();
	
	bool breaking = doInitialSetup(iter1,iter2,info,command, country, firstConfirmedDate, firstDeathDate, firstRecoveredDate, count, dayCount);
	
	if(breaking == true)
		return;
		
	while(iter2 != info.at(country).history.end()){
		
		if(count == 8 && iter1 != info.at(country).history.end()){
			cout << " .\n .\n ." << endl;
			flag = true;
			count = 0;
		}
		if(iter1 == info.at(country).history.end()){
			flag = false;
			//flag2 = true;
		}
		
		if(command == "c" && flag == false ){
			cout << iter2->first.substr(16,iter2->first.find(".csv")-16) << " (day " << dayCount << "): " << iter2->second.confirmed << endl;
			count++;
		}
		else if(command == "d" && flag == false){
			cout << iter2->first.substr(16,iter2->first.find(".csv")-16) << " (day " << dayCount << "): " << iter2->second.deaths << endl;
			count++;
		}
		else if(command == "r" && flag == false){
			cout << iter2->first.substr(16,iter2->first.find(".csv")-16) << " (day " << dayCount << "): " << iter2->second.recovered << endl;
			count++;
		}
		
		iter1++;
		iter2++;
		dayCount++;
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
