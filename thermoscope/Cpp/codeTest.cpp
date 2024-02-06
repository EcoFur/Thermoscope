#include <bits/stdc++.h>
using namespace std;

struct Tmp{
  short tmp;
  long long time;
}tmp[1030];
int tmpDig=0;

Tmp readTmp(int dig);
void writeTmp(short data);

int main()
{
	for(int i=0;i<1500;i++){
		writeTmp(short(i));
	}
	for(int i=0;i<1024;i++)
		cout << endl << i << " " <<readTmp(i).tmp << " " << tmp[i].tmp;
	return 0;
}

Tmp readTmp(int dig)
{
  return tmp[tmpDig-dig<0?tmpDig-dig+1024:tmpDig-dig];
}
void writeTmp(short data)
{
	tmpDig++;
	if(tmpDig>=1024)
	tmpDig=0;
	tmp[tmpDig].tmp=data;
	tmp[tmpDig].time=0;
}