#ifndef __SIMPLE_CONFIG_H__
#define __SIMPLE_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

using namespace std;

const static int MAX_SECT_NUM = 128;

class SimpleConfig 
{

public:
	SimpleConfig() {}
	SimpleConfig(const char *pConfig) { open(pConfig); }
	virtual ~SimpleConfig(){}
	
	int open(const char *pConfig)
	{
		FILE *fp = fopen(pConfig, "r");
		if (fp == NULL) {
			return -1;
		}
		char buf[1024];
		int nId = 0;
		map<string, int>::iterator iter;
		while (fgets(buf, sizeof(buf), fp)) {
			str_trim(buf);
			int nLen = strlen(buf);
			if (nLen == 0) continue;
			if (buf[0] == '#' ||
				(nLen > 1 && buf[0] == '/' && buf[1] == '/'))
			{
				continue;
			}

			string strKey = "", strValue = "", strSect = "";
			if (get_sect(buf, strSect) == 0) {
				if ((iter = _index.find(strSect)) == _index.end()) {
					if (++nId >= MAX_SECT_NUM) {
						fprintf(stderr, "too many section\n");
						return -1;
					}
					_index.insert(map<string, int>::value_type(strSect, nId));
				}
				else {
					fprintf(stderr, "%s is repeat, pls check\n", strSect.c_str());
					return -1;
				}
				continue;
			}

			if (split(buf, strKey, strValue) != 0) {
				continue;
			}

			if (strKey.size() > 0) {
				_szDict[nId][strKey] = strValue;
			}
		}
		fclose(fp);
		return 0;
	}

	string operator [](const string& strKey)
	{
		return read(strKey);
	}	

	string read(const string &strKey)
	{
		string strResult = _szDict[0][strKey];
		return strResult;
	}

	string read(const string &strSect, const string &strKey)
	{
		map<string, int>::iterator iter = _index.find(strSect);
		if (iter == _index.end()) {
			return "";
		}
		string strResult = _szDict[iter->second][strKey];
		return strResult;
	}	

private:
	map<string, string> _szDict[MAX_SECT_NUM];
	map<string, int> _index;

private:
	int get_sect(char *pLine, string &strSect)
	{
		char *p1 = strchr(pLine, '[');
		if (p1 == NULL) return -1;
		char *p2 = strchr(pLine, ']');
		if (p2 == NULL) return -2;
		*p2 = '\0';
		p1++;
		str_trim(p1);
		strSect = p1;
		return 0;
	}

	int split(char *pLine, string &strKey, string &strValue)
	{
		char *p = strchr(pLine, '=');
		if (p == NULL) return -1;
		*p = '\0';
		str_trim(pLine);
		strKey = pLine;
		p++;
		str_trim(p);
		strValue = p;
		return 0;
	}

	bool is_chinese(char ch)
	{
		return (((unsigned char)ch) & 0x80) != 0;
	}

	char *str_ltrim(char *str) 
	{       
		char *p;
		char *end_ptr; 
		int dest_len;

		end_ptr = str + strlen(str);
		for (p = str; p < end_ptr; p++) {
			if (is_chinese(*p)) { 
				break; 
			}
			else if (*p > 32) {
				break;
			}
		}

		if (p == str) {
			return str;
		}
		dest_len = (end_ptr - p) + 1; //including \0
		memmove(str, p, dest_len);
		return str;
	}       

	char *str_rtrim(char *str)
	{           
		char *p;
		char *last_ptr;

		int len = strlen(str);
		if (len == 0) {
			return str;
		}               

		last_ptr = str + len - 1;
		for (p = last_ptr; p >= str; p--) {
			if (is_chinese(*p)) {
				break;
			}       
			else if (*p > 32) {
				break;  
			}           
		}               
		if (p != last_ptr) {
			*(p + 1) = '\0';
		}       
		return str;
	}       

	char *str_trim(char *str)
	{
		str_rtrim(str);
		str_ltrim(str);
		return str;
	}
};

#endif // __SIMPLE_CONFIG_H__

