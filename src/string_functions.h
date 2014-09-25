#ifndef GUARD_string_functions_h
#define GUARD_string_functions_h
using namespace std;

struct host_ip {
    string ip;
    string host;
};

struct stats_nicks {
    string ip;
    string name;
    string id;
};

string padZeroes(const size_t&);
string addZeroes(string&);
struct _compare {
  bool operator() (const string& aa, const string& bb) const
  {
    string a = aa;
    string b = bb;
    addZeroes(a);
    addZeroes(b);
    return a < b;
  }
};

vector<string> split(const string&);
string UpStr(string&);
vector<string> file_list();
bool isip_in_ml (map<string,string>&, string&);
bool isnick_in_ml(map<string,string>&,string&, string);
bool isin_dont_lookup(vector<string>&,string&);
string clean_nick(string&); //remove spaces
int isInHostCache(vector<host_ip>&,string&);
bool add_to_ml(map<string,string>&, string&, string&);
void parse_irc_logs(map<string,string>&,vector<string>&,vector<host_ip>&,string&);
void parse_stats_logs(vector<stats_nicks>&, stats_nicks&, string&);

#endif
