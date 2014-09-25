#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <stdio.h>
#include <windows.h>
#include <map>
#include "dns_lookup.h"
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

//can use sets instead of these 2 custom structures

string padZeroes(const size_t& i) {
    if (i == 3) return "";
    if (i == 1) return "00";
    if (i == 2) return "0";
}
string addZeroes(string& a) {
    size_t founda1=a.find_first_of(".");
    size_t founda2=a.find_first_of(".",founda1+1);
    size_t founda3=a.find_first_of(".",founda2+1);
    a.insert(founda3+1,padZeroes(a.size()-founda3-1));
    a.insert(founda2+1,padZeroes(founda3-founda2-1));
    a.insert(founda1+1,padZeroes(founda2-founda1-1));
    a.insert(0,padZeroes(founda1));
    return a;
}
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

vector<string> split(const string& s)
{
    vector<string> ret;
    typedef string::size_type string_size;
    string_size i = 0;
    while (i != s.size()) {
     while (i != s.size() && isspace(s[i]))
         ++i;
     string_size j = i;
     while (j != s.size() && !isspace(s[j]))
	     j++;
         if (i != j) {
             ret.push_back(s.substr(i, j - i));
             i = j;
         }
    }
    return ret;
}
//modify this to remove toReturn string completely
string UpStr(string& text) {
    string toReturn(text);
    for(int i=0;i<text.length();++i) {
        toReturn[i]=toupper(text[i]);
    }
    return toReturn;
}
vector<string> file_list()
{
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   char DirSpec[MAX_PATH] = "*";
   DWORD dwError;
   vector<string> dir_names;
   hFind = FindFirstFile(DirSpec, &FindFileData);
      dir_names.push_back(FindFileData.cFileName);
      while (FindNextFile(hFind, &FindFileData) != 0)
      {
         dir_names.push_back(FindFileData.cFileName);

      }
      dwError = GetLastError();
      FindClose(hFind);
    return dir_names;
}

bool isip_in_ml(map<string,string>& ml, string& ip) {
    if (ml[ip] != "") return true;
    else return false;
}

bool isnick_in_ml(map<string,string>& ml, string& ip, string nick) {
    if (UpStr(ml[ip]).find(UpStr(nick)) != string::npos) return true;
    else return false;
}

//modify this for efficiency
bool isin_dont_lookup(vector<string>& dont_lookup, string& lookup) {
    int i = 0;
    while ( i != dont_lookup.size()  ) {
        if ( lookup == dont_lookup[i] ) {
            return true;
        }
        ++i;
    }
    return false;
}
//remove spaces from nicks and other weird characters
string clean_nick(string& nick) {
    if ( nick.substr(0,1) == "@" || nick.substr(0,1) == "+" ) return nick.substr(1);
    else return nick;    
}
int isInHostCache(vector<host_ip>& hostCache,string& s) {
    int i = 0;
    while ( i != hostCache.size()  ) {
        if ( hostCache[i].host == s ) {
            return i;
        }
        ++i;
    }
    return -1;
}
bool add_to_ml(map<string,string>& ml, string& nick, string& ip) {
    if ( isip_in_ml(ml,ip) != true ) ml[ip]=clean_nick(nick);
    else if (isnick_in_ml(ml,ip,clean_nick(nick)) != true )
        ml[ip]= ml[ip] + " " + clean_nick(nick);
}
void parse_irc_logs(map<string,string>& ml,vector<string>& dont_lookup,vector<host_ip>& hostCache,string& s) {
    if ( s.find_first_of("@") != string::npos ) {
        vector<string> v = split(s);
        if (v.size() > 4 ) {
            int pos = 0;
            if (v[3].substr(0,1) == "(") pos = 3;
            if (v[4].substr(0,1) == "(") pos = 4;
            if (pos == 3 || pos == 4) {
                int findpos = v[pos].find("@");
                string dns_string = v[pos].substr(findpos+1,v[pos].size() - findpos - 2 );
                if ( isin_dont_lookup(dont_lookup,dns_string) != true && dns_string.find(".user") == string::npos && dns_string.find("chattingaway") == string::npos ) {
                  int test = isInHostCache(hostCache,dns_string);
                  //changethisstuff, including all ofhostcache, tomake it more efficient
                  string dns_return = "";
                  if (test >= 0) dns_return = hostCache[test].ip;
                  else {
                    char dns_char[50];
                    strcpy(dns_char, dns_string.c_str());
                    dns_return = dns_lookup(dns_char);
                  }
                  if ( dns_return != "failed" ) add_to_ml(ml,v[pos -1],dns_return);
                  else dont_lookup.push_back(dns_string);
                }
            }
        }
    }
}
void parse_stats_logs(vector<stats_nicks>& stats_ips, stats_nicks& tmp, string& s) {
    if ( s.find("player\tConnect") != string::npos ) {
        int name = s.find("player\tConnect") +15;
        int name2 = s.find_first_of("\t",name);
        int id = s.find("\t",name2+1);
        //cout << s.substr(name,name2-name) << " " << s.substr(id+1) << endl; 
        tmp.name=s.substr(name,name2-name);
        tmp.id=s.substr(name2+1,id-name2-1);
        //cout << tmp.id << endl;
        stats_ips.push_back(tmp);
    }
    if ( s.find("player\tIP") != string::npos ) {
        int idd = s.find("player\tIP") +10;
        int idd2 = s.find_first_of("\t",idd);
        //cout << s.substr(idd,idd2-idd) << " " << s.substr(idd2+1) << endl;
        int i = 0;
        while (i != stats_ips.size()) {
            if (stats_ips[i].id == s.substr(idd,idd2-idd)) {
              stats_ips[i].ip = s.substr(idd2+1);
              //cout << stats_ips[i].ip << " " << stats_ips[i].name << endl;
              break;
            }
            else ++i;
        }
    }
}
