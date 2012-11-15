
#include <map>
#include "rbtree.h"
#include "rbmap.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
//#include <xtree>
#include <vector>
#include <time.h>

using namespace std;


string printTree(rbtree<_rbTraits<int,int, less<int>>> *tree, int spaces)
{
	string sp = "";
	for (int i = 0; i < spaces; i++)
		sp += "        ";
	if (tree->is_nil())
		return sp + ".";
	string right = printTree(tree->right(), spaces + 1);
	string left = printTree(tree->left(), spaces + 1);
	char *num = new char[15];
	char *num2 = new char[15];
	char *num3 = new char[15];
	itoa(tree->data()->first,num,10);
	itoa(tree->data()->second,num2,10);
	itoa(tree->rank(),num3,10);
	string elem = num;
	string elem2 = num2;
	string elem3 = num3;
	elem = sp + "[" + elem + ", " + elem2 + "] " + elem3;
	//elem = sp + "(" + elem + ")";
	return right + "\n" + elem + "\n" + left;
}


bool alright(rbtree<_rbTraits<int,int, less<int>>> *tree)
{
	if (tree->is_nil())
	{
		return true;
	}
	if (!tree->right()->is_nil() && !alright(tree->right()))
		return false;
	if (!tree->left()->is_nil() && !alright(tree->left()))
		return false;
	if (tree->rank()-tree->right()->rank() < 0)
	{
		cout << "right < 0" << endl;
		//printTree(tree, 0);
		cout << endl;
		return false;
	}
	if (tree->rank()-tree->left()->rank() < 0)
	{
		cout << "left < 0" << endl;
		//printTree(tree, 0);
		cout << endl;
		return false;
	}
	if (tree->rank()-tree->right()->rank() > 2)
	{
		cout << "right > 2" << endl;
		//printTree(tree, 0);
		cout << endl;
		return false;
	}
	if (tree->rank()-tree->left()->rank() > 2)
	{
		cout << "left > 2" << endl;
		//printTree(tree, 0);
		cout << endl;
		return false;
	}
	if (tree->right()->is_nil() && tree->left()->is_nil())
	{
		if (tree->rank() == 1)
		{
			cout << "2,2-leaf" << endl;
			return false;
		}
	}
	if (tree->rank() - tree->right()->rank() == 2 && tree->rank() - tree->left()->rank() == 2)
	{
		//cout << "2,2-OK" << endl;
	}
	return true;
}

struct str
{
	const char *s;
	bool operator<(const str s2) const
	{
		return strcmp(s, s2.s) < 0;
	}
};

ofstream fout;

int test2()
{
	int amount = 100000;
	int num[100000];
	map <int, int> m;	
	rbmap <int, int> rm;
	srand(time(0));
	int ran;
	int k;
	for (int i = 0; i < amount; i++)
	{
		fout.open("height21.txt", ios_base::out|ios_base::app);
		ran = int(4000000000*((double)rand()/32768));
		num[i] = ran;
		m.insert(std::pair<int, int>(ran, i));
		rm.insert(std::pair<int, int>(ran, i));
		//fout << m.height() << " " << rm.height() << endl;
		fout.close();	
	}
	int am2 = 5*amount/6;
	for (int i = 0; i < am2; i++)
	{
		fout.open("height22.txt", ios_base::out|ios_base::app);
		ran = int((amount)*((double)rand()/32768));
		k = num[ran];
		num[ran] = num[amount-1];
		amount--;
		m.erase(k);
		rm.erase(k);
		//fout << m.height() << " " << rm.height() << endl;
		fout.close();	
	}
	for (int i = 0; i < am2; i++)
	{
		fout.open("height23.txt", ios_base::out|ios_base::app);
		ran = int(4000000000*((double)rand()/32768));
		m.insert(std::pair<int, int>(ran, i));
		rm.insert(std::pair<int, int>(ran, i));
		//fout << m.height() << " " << rm.height() << endl;
		fout.close();	
	}

	return 0;
}


int test3()
{
	const int am = 100000;
	int num[am];
	map <int, int> m;	
	rbmap <int, int> rm;
	srand(time(0));
	int ran;
	int k;
	int amount = 1000;
	for (int i = 0; i < 1000; i++)
	{
		ran = int(4000000000*((double)rand()/32768));
		num[i] = ran;
		m.insert(std::pair<int, int>(ran, i));
		rm.insert(std::pair<int, int>(ran, i));
	}
	for (int i = 0; i < 90000; i++)
	{
		fout.open("height3.txt", ios_base::out|ios_base::app);
		if (int((2)*((double)rand()/32768)) == 1)
		{
			amount++;
			fout << amount << " ";
			ran = int(4000000000*((double)rand()/32768));
			num[i] = ran;
			m.insert(std::pair<int, int>(ran, i));
			rm.insert(std::pair<int, int>(ran, i));
			//fout << m.height() << " " << rm.height() << endl;
		}
		else {
			amount--;
			fout << amount << " ";
			ran = int((amount)*((double)rand()/32768));
			k = num[ran];
			num[ran] = num[amount];
			m.erase(k);
			rm.erase(k);
			//fout << m.height() << " " << rm.height() << endl;
		}
		fout.close();	
	}

	return 0;
}


int main()
{
	const int max = 100000000;
	//fout.open("speedtest.txt", ios_base::out|ios_base::app);

	srand(time(0));
	int k, *v, *all, n, u;
	__int64 begin;
	map<int, int> M;
	rbmap<int, int> N;


	all = new int[max];
	for (int i = 0; i < max; i++)
	{
		k = int(4000000000*((double)rand()/32768));
		all[i] = k;
	}

	begin = clock();
	for (int i = 0; i < max; i++)
	{
		M.insert(pair<int,int>(all[i],1));
	}
	cout << clock() - begin << " ";
	begin = clock();
	for (int i = 0; i < max; i++)
	{
		N.insert(pair<int,int>(all[i],1));
	}
	cout << clock() - begin << endl;


	int d;
	int count = max;
	int *all2 = new int[max];
	int *all3 = new int[max];
	for (int i = 0; i < max; i++)
	{
		all3[i] = all[i];
	}
	
	begin = clock();

	for (int i = 0; i < max; i++)
	{
		d = int(count*((double)rand()/32768));
		all2[i] = d;
		M.erase(all[d]);
		all[d] = all[count];
		count--;
	}
	cout << clock() - begin << " ";
	begin = clock();
	count = max;
	for (int i = 0; i < max; i++)
	{
		N.erase(all3[all2[i]]);
		all3[all2[i]] = all[count];
		count--;
		d = int(count*((double)rand()/32768));
		all2[i] = d;
	}
	cout << clock() - begin <<  endl;
	
	
	int *all4 = new int[max];
	begin = clock();
	for (int i = 0; i < max; i++)
	{
		d = int(4000000000*((double)rand()/32768));
		all4[i] = d;
		M.insert(pair<int,int>(d,1));
	}
	cout << clock() - begin << " ";
	begin = clock();
	for (int i = 0; i < max; i++)
	{

		N.insert(pair<int,int>(all4[i],1));
		d = int(4000000000*((double)rand()/32768));
		all4[i] = d;
	}
	cout << clock() - begin <<  endl;


	return 0;
}


/*
int main()
{

	const int max = 100000000;
	fout.open("speedtestF.txt", ios_base::out|ios_base::app);

	srand(time(0));
	int k, *v, *all, n, u;
	__int64 begin;
	map<int, int> M;
	rbmap<int, int> N;


	all = new int[max];
	for (int i = 0; i < max; i++)
	{
		k = int(4000000000*((double)rand()/32768));
		all[i] = k;
	}

	begin = clock();
	for (int i = 0; i < max; i++)
	{
		M.insert(pair<int,int>(all[i],1));
	}
	fout << clock() - begin << " ";
	begin = clock();
	for (int i = 0; i < max; i++)
	{
		N.insert(pair<int,int>(all[i],1));
	}
	fout << clock() - begin << endl;


	int d;
	int count = max;
	int *all2 = new int[max];
	int *all3 = new int[max];
	for (int i = 0; i < max; i++)
	{
		all3[i] = all[i];
	}
	

	__int64 time = 0;

	for (int i = 0; i < max; i++)
	{
		d = int(count*((double)rand()/32768));
		all2[i] = d;
		begin = clock();
		M.erase(all[d]);
		time += (clock() - begin);
		all[d] = all[count];
		count--;
	}
	fout << time << " ";
	time = 0;
	count = max;
	for (int i = 0; i < max; i++)
	{
		begin = clock();
		N.erase(all3[all2[i]]);
		time += (clock() - begin);
		all3[all2[i]] = all[count];
		count--;
	}
	fout << time <<  endl;

	
	int *all4 = new int[max];
	time = 0;
	for (int i = 0; i < max; i++)
	{
		begin = clock();
		d = int(4000000000*((double)rand()/32768));
		time += (clock() - begin);
		all4[i] = d;
		M.insert(pair<int,int>(d,1));
	}
	fout << time << " ";
	begin = clock();
	for (int i = 0; i < max; i++)
	{
		begin = clock();
		N.insert(pair<int,int>(all4[i],1));
		time += (clock() - begin);
		d = int(4000000000*((double)rand()/32768));
		all4[i] = d;
	}
	fout << time <<  endl;


	return 0;
}*/