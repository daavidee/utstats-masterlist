#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
#include "dns_lookup.h"
#include "string_functions.h"

using namespace std;

map<string,string> make_ml_vector(string ml_file)
{
    map<string,string> vec_output;
    string s;
    ifstream in_file(ml_file.c_str());
    while (getline(in_file, s)) {
        if ( s != "" ) {
            int pos = s.find_first_of(" ");
            vec_output[s.substr(0,pos)] = s.substr(pos+1);
        }
    }
    in_file.close();
    return vec_output;
}
vector<string> make_dont_lookup_vector(string ml_file) {
    vector<string> vec_output;
    string s;
    ifstream in_file(ml_file.c_str());
    while (getline(in_file, s)) {
        if ( s != "" ) {
            vec_output.push_back(s);
        }
    }
    in_file.close();
    return vec_output;
}

//make a map of hosts and ips for every ip looked up, so dont need dnsrequests for repeats
int main(int argc, char *argv[])
{
  clock_t start;
  map<string,string> ml = make_ml_vector("masterlist.txt");
  vector<string> dont_lookup = make_dont_lookup_vector("dont_lookup.txt");
  vector<host_ip> hostCache;
 
  vector<string> dir_names = file_list(); 
  //go deeper into subfolders, remove files from list which arent .log

  int j = 0;
  while (j != dir_names.size()) {
    ifstream infile(dir_names[j].c_str());
    cout << dir_names[j].c_str() << endl;
    string s;
    if (dir_names[j].substr(0,13) == "Unreal.ngLog." ) {
      vector<stats_nicks> stats_ips;
      stats_nicks tmp;
      while (getline(infile, s)) {  
        if (s.find("game_start") != string::npos) break;  
        parse_stats_logs(stats_ips, tmp, s);
      }
      int p = 0;
      while (p != stats_ips.size()) {
        if (stats_ips[p].ip != "") add_to_ml(ml,stats_ips[p].name,stats_ips[p].ip);
        ++p;
      }
      
    }
    else {
      while (getline(infile, s)) {
        parse_irc_logs(ml,dont_lookup,hostCache,s);
      }
    }
    infile.close();
    ++j;
  }


  ofstream outfile_dont_lookup("dont_lookup.txt");
  for (int i = 0; i != dont_lookup.size(); i++ )
    outfile_dont_lookup << dont_lookup[i] << endl;
  outfile_dont_lookup.close();
  
  ofstream outfile_ml("masterlist.txt");
  map<string,string,_compare> ml2;
  map<string,string>::iterator it;
  cout << "sorting...<60 secs" << endl;
  for ( it=ml.begin(); it != ml.end(); it++ )
    ml2[(*it).first]=(*it).second;
  for ( it=ml2.begin(); it != ml2.end(); it++ )
    outfile_ml << (*it).first << " " << (*it).second << endl;
  outfile_ml.close();


  double diff;
  diff = ( clock() - start ) / (double)CLOCKS_PER_SEC;
  cout << "Total execution time: " << diff << endl;
  system("pause");
  return 0;
}
