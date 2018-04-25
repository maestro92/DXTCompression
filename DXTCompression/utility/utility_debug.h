#ifndef UTILITY_DEBUG_H_
#define UTILITY_DEBUG_H_
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const string clientDebugPrefix = "									";

namespace utl
{
	/// utl_debug.cpp
	void debugLn(int l = 1);
	void debugLn(string s, int l = 1);
	void debugLn(string s, bool b, int l = 1);
	void debugLn(string s, char c, int l = 1);
	void debugLn(string s, string s2, int l = 1);
	void debugLn(string s, unsigned int i, int l = 1);
	void debugLn(string s, int i, int l = 1);
	void debugLn(string s, float f, int l = 1);


	void debug(string s);
	void debug(string s, bool b);
	void debug(string s, char c);
	void debug(string s, string s2);
	void debug(string s, const char* s2);
	void debug(string s, unsigned int i);
	void debug(string s, int i);
	void debug(string s, float f);
	void debug(string s, double d);

	template <class T>
	void debug(string s, vector<T> v);

	template <class T>
	void debug(string s, vector< vector<T> > v);
}


template <class T>
void utl::debug(string s, vector<T> v)
{
	cout << s << endl;
	for (int i = 0; i < v.size(); i++)
		cout << v[i] << " ";
	cout << endl;
}

template <class T>
void utl::debug(string s, vector< vector<T> > v)
{
	cout << s << endl;
	for (int y = 0; y < v.size(); y++)
	{
		for (int x = 0; x < v[y].size(); x++)
			cout << setw(20) << left << v[y][x];
		cout << endl;
	}
	cout << endl;
}

#endif
