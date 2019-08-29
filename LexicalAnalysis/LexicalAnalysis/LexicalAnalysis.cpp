//LexicalAnalysis.cpp
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

using namespace std;

/****************************************************************************************/
//全局变量，保留字表
static char reserveWord[10][20] = {
	 "include","stdio","print","void","main","int","while","if","else","const"
};
//界符运算符表,根据需要可以自行增加
static char operatorOrDelimiter[16][10] = {
	"+", "-", "*", "/", "=", ">","<",  ":","(", ")","{",
	"}",";"," >= ", " <= ", " == "
};

static  char IDentifierTbl[1000][50] = { "" };//标识符表
static  char ConstTbl[100][50] = { "" };//常数表

/********查找保留字*****************/
int searchReserve(char reserveWord[][20], char s[])
{
	for (int i = 0; i < 10; i++)
	{
		if (strcmp(reserveWord[i], s) == 0) {//若成功查找，则返回类别码
			return i + 3;//返回类别码
		}
	}
	return -1;//否则返回-1，代表查找不成功，即为标识符
}
/*********************判断是否为字母********************/
bool IsLetter(char letter) {//注意C语言允许下划线也为标识符的一部分可以放在首部或其他地方
	if (letter >= 'a' && letter <= 'z' || letter >= 'A' && letter <= 'Z' || letter == '_')
		return true;
	else
		return false;
}
/*****************判断是否为数字************************/
bool IsDigit(char digit) {
	if (digit >= '0' && digit <= '9')
		return true;
	else
		return false;
}
/********************编译预处理，取出无用的字符和注释**********************/
void filterResource(char r[], int pProject) {
	char tempString[10000];
	int count = 0;
	for (int i = 0; i <= pProject; i++)
	{
		if (r[i] == '/' && r[i + 1] == '/')
		{
			while (r[i] != '\n')
				i++;
		}
		else if (r[i] == '/' && r[i + 1] == '*')
		{
			i += 2;
			while (r[i] != '*' || r[i + 1] != '/')
			{
				i++;//继续扫描
				if (r[i] == '#') {
					cout << "注释出错，没有找到 */，程序结束！！！" << endl;
					exit(0);
				}
			}
			i += 2;//跨过“*/”
		}
		else if (r[i] != '\n' && r[i] != '\t' && r[i] != '\v' && r[i] != '\r')
			tempString[count++] = r[i];			
	}
	tempString[count] = '\0';
	strcpy_s(r, 10000,tempString);//产生净化之后的源程序
}
/****************************分析子程序，算法核心***********************/
void Scanner(int& syn, char resourceProject[], char token[], int& pProject) {
	int i, count = 0;//count用来做token[]的指示器，收集有用字符
	char ch;//作为判断使用
	ch = resourceProject[pProject];
	while (ch == ' ')
	{
		pProject++;
		ch= resourceProject[pProject];
	}
	for (int i = 0; i < 20; i++) {
		token[i] = '\0';
	}
	if (IsLetter(resourceProject[pProject])) {
		token[count++] = resourceProject[pProject];//收集
		pProject++;//下移
		while (IsLetter(resourceProject[pProject]) || IsDigit(resourceProject[pProject])) {//后跟字母或数字
			token[count++] = resourceProject[pProject];//收集
			pProject++;//下移
		}//多读了一个字符既是下次将要开始的指针位置
		token[count] = '\0';
		syn = searchReserve(reserveWord, token);//查表找到种别码
		if (syn == -1) {//若不是保留字则是标识符
			syn = 1;//标识符类别码
		}
		return;
	}
	else if (IsDigit(resourceProject[pProject])) {
		while (IsDigit(resourceProject[pProject])) {//后跟数字
			token[count++] = resourceProject[pProject];//收集
			pProject++;
		}//多读了一个字符既是下次将要开始的指针位置
		token[count] = '\0';
		syn = 2;//常数
	}
	else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '>' || ch == '<' || ch == ':'
		|| ch == '(' || ch == ')' || ch == '{' || ch == '}'||ch==';') {
		token[0] = resourceProject[pProject];
		token[1] = '\0';//形成单字符串
		for (i = 0; i < 13; i++) {//查运算符界符表
			if (strcmp(token, operatorOrDelimiter[i]) == 0) {
				syn = 13 + i;//获得类别码
				break;//查到即推出
			}
		}
		pProject++;//指针下移，为下一扫描做准备
		return;
	}
	else if (resourceProject[pProject] == '<')
	{
		pProject++;//后移，超前搜索
		if (resourceProject[pProject] == '=')
			syn = 26;
		else
		{
			pProject--;
			syn = 19;
		}
		pProject++;//指针下移
		return;
	}
	else if (resourceProject[pProject] == '>')
	{
		pProject++;
		if (resourceProject[pProject] == '=')
			syn = 27;
		else
		{
			pProject--;
			syn = 18;
		}
		pProject++;
		return;
	}
	else if (resourceProject[pProject] == '=')
	{
		pProject++;
		if (resourceProject[pProject] == '=')
			syn = 28;
		else
		{
			pProject--;
			syn = 17;
		}
		pProject++;
		return;
	}
	else if(resourceProject[pProject] == '#')
		syn = 0;//类别码为0
	else
	{
		printf("error：there is no exist %c \n", ch);
		//exit(0);
		pProject++;
		return;
	}
}

int main()
{
	//打开一个文件，读取其中的源程序
	char resourceProject[10000];
	char token[20] = { 0 };
	int syn = -1, i;//初始化
	int pProject = 0;//源程序指针
	FILE* fp, * fp1,*fp2,*fp3;
	if ((errno=fopen_s(&fp,"E:\\1.txt", "r"))!=0) {//打开源程序
		cout << "can't open this file";
		exit(0);
	}
	resourceProject[pProject] = fgetc(fp);
	while (resourceProject[pProject] != '#') {//将源程序读入resourceProject[]数组
		pProject++;
		resourceProject[pProject] = fgetc(fp);
	}
	resourceProject[++pProject] = '\0';
	fclose(fp);
	cout << endl << "源程序为:" << endl;
	cout << resourceProject << endl;
	//对源程序进行过滤
	filterResource(resourceProject, pProject);
	cout << endl << "过滤之后的程序:" << endl;
	cout << resourceProject << endl;
	pProject = 0;//从头开始读
	if ((errno = fopen_s(&fp1, "E:\\token.txt", "w+")) != 0) {//打开源程序
		cout << "can't open this file";
		exit(0);
	}
	if ((errno = fopen_s(&fp2, "E:\\identifier.txt", "w+")) != 0) {//打开源程序
		cout << "can't open this file";
		exit(0);
	}
	if ((errno = fopen_s(&fp3, "E:\\constant.txt", "w+")) != 0) {//打开源程序
		cout << "can't open this file";
		exit(0);
	}
	while (syn != 0)
	{
		//启动扫描
		Scanner(syn, resourceProject, token, pProject);
		if (syn == 1) {//标识符
			for (i = 0; i < 1000; i++)//插入标识符表中
			{

				if (strcmp(IDentifierTbl[i], token) == 0) {//已在表中
					break;
				}
				if (strcmp(IDentifierTbl[i], "") == 0) {//查找空间
					strcpy_s(IDentifierTbl[i], token);
					break;
				}
			}
			printf("(标识符  ,%s)\n", token);
			fprintf(fp1, "(标识符   ,%s)\n", token);
		}
		else if (syn >= 3 && syn <= 12) {//保留字
			printf("(%s   ,  %d)\n", reserveWord[syn - 3],syn);
			fprintf(fp1, "(%s   ,  %d)\n", reserveWord[syn - 3],syn);
		}
		else if (syn == 2) {//const 常数
			for (i = 0; i < 100; i++)//插入表中
			{

				if (strcmp(ConstTbl[i], token) == 0) {//已在表中
					break;
				}
				if (strcmp(ConstTbl[i], "") == 0) {//查找空间
					strcpy_s(ConstTbl[i], token);
					break;
				}
			}
			printf("(常数   ,   %s)\n", token);
			fprintf(fp1, "(常数   ,   %s)\n", token);
		}
		else if (syn >= 13 && syn <= 28) {
			printf("(%s   ,   %d)\n", operatorOrDelimiter[syn - 13],syn);
			fprintf(fp1, "(%s   ,   %d)\n", operatorOrDelimiter[syn - 13],syn);
		}
	}
	for (i = 0; i < 100; i++) {//插入标识符表中
		printf("第%d个标识符：  %s\n", i + 1, IDentifierTbl[i]);
		fprintf(fp2, "第%d个标识符：  %s\n", i + 1, IDentifierTbl[i]);
	}
	for (i = 0; i < 100; i++) {//插入常数表中
		printf("第%d个常数：  %s\n", i + 1, ConstTbl[i]);
		fprintf(fp3, "第%d个常数：  %s\n", i + 1, ConstTbl[i]);
	}
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	return 0;
}