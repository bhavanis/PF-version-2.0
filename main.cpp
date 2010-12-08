#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "json_spirit_reader_template.h"
// #include "json_spirit_value_template.h"
using namespace std;
using namespace json_spirit;

void clean_url(string& url)
{
	for(int i = 0; i < url.size(); i++)
	{
		switch(url[i])
		{
			case ' ':
			case '\"':
			case '<':
			case '>':
			case '#':
			case '%':
				break;
		}
	}
}

void FailFunc( int *fail, char *pattern) {
	int m = strlen(pattern+1);
	fail[1] = 0;
	int j = 0;
	for ( int i = 2; i <= m; i++ ) {
		while ( j > 0 && pattern[j+1] != pattern [i] ) 
				j = fail[j];
		if ( pattern[j + 1] == pattern[i] ) {
				j = j + 1;
		}
		fail[i] = j;
	}
} 
void KMP ( char *patt, char *text,string &str) {
	int fail[1000];
	//FailFunc( fail, patt);
	int i,j;
	i = 0; // text
	j = 0; // pattern
	int count = 0;
	bool match_found = false;
	bool space_found = true;
	int m = strlen ( patt +1);
	int n = strlen ( text +1);
	FailFunc( fail, patt);
	int q = 0;
	
	for ( i = 1; i <= n; i++ ) {
		if (space_found && match_found) {
			//string tmp;
			//str.push_back(tmp);
			space_found = false;
		}
		if (text[i] == ' ') {
			space_found = true;
			if (match_found)
				return;
		} 
		if (!space_found ) {
			str.push_back(text[i]);
		}
		while ( q > 0 && patt[q + 1] != text[i] ) {
			q = fail[q];
		}
		if ( patt[q+1] == text[i]) {
			q = q + 1;
		}
		if ( q == m ) {
			//str_list.push_back(i-m);
			q = fail[q];
			match_found = true;
			space_found = true;
		}
	}
	return ;
}
void getUrl(string text,string &str1) {
	int i = 0;
	char *str;
	str = new char [text.size()+5];
	str[0] = ' ';
	strcpy (str+1, text.c_str());
	char pattern[]=" http://bit.ly/";
	cout << "pattern: "<<pattern<<"text : "<<str<<endl;
	KMP(pattern,str,str1);
	
}
void parse_current(const string& infile,map <string,string> imp)
{
	ifstream instream(infile.c_str());
	Value value;
	read_stream(instream, value);
	value = value.get_obj()[0].value_;
	
	//value = value.get_obj()[0].value_;
	//vector <string> str_list;
	const Array& arr = value.get_array();
	for(int i = 0; i < arr.size(); i++)
	{
		string str;
		cout << "User : " << arr[i].get_obj()[3].value_.get_str() << " Posted Text :" <<arr[i].get_obj()[7].value_.get_str()<<endl;
		getUrl(arr[i].get_obj()[7].value_.get_str(),str);
		string str_tmp = "curl -sL \'http://api.bit.ly/v3/expand?shortUrl=http%3A%2F%2Fbit.ly%2F"+str+"&login=pvrks&apiKey=R_79ee1804a8d8e2c7736fc965fb64fcfd&format=json\' >1.json";
		const char *str_tmp_c = str_tmp.c_str();	
		system(str_tmp_c);
		ifstream instream_tmp("1.json");
		Value value1;
		read_stream(instream_tmp, value1);
		value1 = value1.get_obj()[2].value_;
		value1 = value1.get_obj()[0].value_;
		const Array& arr1 = value1.get_array();
		cout << " Image Url Posted by User is: "<<arr1[0].get_obj()[1].value_.get_str() <<endl;
		imp[arr[i].get_obj()[3].value_.get_str()] = arr1[0].get_obj()[1].value_.get_str();
		return;
	}
}

bool err_is_zero()
{
	system("echo -n Err; cat err");
	ifstream instream("err");
	int err;
	instream >> err;
	return (err == 0);
}

bool face_matcher(string url_missed,string url_found){
		//implementation needed
		return true;
}

void match_iden_missed(string identity,string missed) {
	map <string,string> imp1,imp2;

	parse_current(missed,imp1);
	parse_current(identity,imp2);
	//implement a better algo if time permits
	map <string, string> :: iterator it1;
	map <string, string> :: iterator it2;
	for(it1 = imp1.begin(); it1 != imp1.end(); it1++)
	{
		for(it2 = imp2.begin(); it2 != imp1.end(); it2++)
		{
			if(face_matcher((*it1).second,(*it2).second)) {
				//if both are same then post it to the respective user
				//oAuth etc 
			}
		}
	}	

}
bool fileisEmpty(string name) {
	int length;
	ifstream filestr;
	filestr.open(name.c_str(), ios::binary); 
	filestr.seekg(0, ios::end); 
	length = filestr.tellg(); 
	filestr.close();
	return (length == 0);
}
int main()
{
	//system("curl -sL \"http://search.twitter.com/trends/current.json\" > current.json");
	system("curl -sL \"http://search.twitter.com/search.json?q=%40blorehelp_test+%23identify\" > tweetstream_iden.json");
	//parse_current("tweetstream.json",);
	system("curl -sL \"http://search.twitter.com/search.json?q=%40blorehelp_test+%23missing\" > tweetstream_missed.json");
	system("diff tweetstream_iden.json tweetstream_iden1.json > 1.txt");
	system("diff tweetstream_missed.json tweetstream_missed1.json > 2.txt");

	if (!fileisEmpty("1.txt") || !fileisEmpty("2.txt")) {
		match_iden_missed("tweetstream_iden.json","tweetstream_missed.json");
		
		system("cp -f tweetstream_iden.json tweetstream_iden1.json");
		system("cp -f tweetstream_missed.json tweetstream_missed1.json");
		
	}
	return 0;
}

