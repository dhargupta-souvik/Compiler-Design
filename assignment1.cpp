#include <bits/stdc++.h>

using namespace std;

int literal_flag=0;
string literal="";
int l_row,l_col,l_tab;

set<string> p_set;	//set of punctuations
set<string> o_set;	//set of operators
set<string> k_set;	//set of keywords

//function to find the set of punctuations from given file
void get_punctuation_set(string filename)
{
	p_set.clear();
	ifstream p_file(filename);
	string line;
	while(getline(p_file,line))
	{
		p_set.insert(line);
	}
}

//function to find the set of operators from given file
void get_operator_set(string filename)
{
	o_set.clear();
	ifstream o_file(filename);
	string line;
	while(getline(o_file,line))
	{
		o_set.insert(line);
	}
}

//function to find the set of keywords from given file
void get_keyword_set(string filename)
{
	k_set.clear();
	ifstream k_file(filename);
	string line;
	while(getline(k_file,line))
	{
		k_set.insert(line);
	}
}

//check if there is a punctuation in the given line at position i
int check_punctuation(string line, int i)
{
	set<string>::iterator it;

	string s="";
	s+=line[i];
	it=p_set.find(s);	//check if the character is in punctuation set

	if(it!=p_set.end())
	{
		return 1;
	}
	else return -1;
}

//check if there is an operator in the given line at position i
int check_operator(string line, int i)
{
	set<string>::iterator it,it2=o_set.end();

	string s="";
	s+=line[i];
	//find if line[i] is in operator set
	it=o_set.find(s);

	if(i<(line.length()-1))
	{
		s+=line[i+1];
		//find if string(line[i]+line[i+1]) is in operator set
		it2=o_set.find(s);
	}

	if(it2!=o_set.end()) //check if the two characters are operators
	{
		return s.length();
	}
	else if(it!=o_set.end()) //check if the 1st character is an operator
	{
		return s.length()-1;
	}
	else return -1;
}

//check if there is a keyword in the given line at position i
int check_keyword(string line, int i)
{
	set<string>::iterator it;

	string s="";int j=i;
	//keep adding characters until delimiter or digit is found
	while(j<line.length()&& line[j]!=' '&& check_punctuation(line,j)==-1 && check_operator(line,j)==-1&& (line[j]<'0'||line[j]>'9'))
	{
		s+=line[j++];
	}

	//check for the keyword in keyword set
	it=k_set.find(s);

	if(it!=k_set.end())
	{
		return s.length();
	}
	else return -1;
}

//check if there is an identifier in the given line at position i
int check_identifier(string line,int i)
{
	string s="";
	//if starts with something other than alphabet or underscore then it is not identifier
	if((line[i]<'a'||line[i]>'z')&&(line[i]<'A'||line[i]>'Z')&&line[i]!='_')
		return -1;
	s+=line[i];
	int j=i+1;
	//add characters to the identifier while any delimiter is encountered
	while(j<line.length()&& line[j]!=' '&& check_punctuation(line,j)==-1 && check_operator(line,j)==-1)
		s+=line[j++];
	return s.length();
}

//check if there is a constant in the given line at position i
int check_constant(string line, int i)
{
	string s="";int j=i;
	//add characters to the constant while it is digit or decimal
	while(j<line.length()&&(line[j]>='0'&&line[j]<='9')||line[j]=='.')
	{
		s+=line[j++];
	}
	if(s.length()>0)
		return s.length();
	return -1;
}

//check and return if there is a literal in the given line at position i
string check_literal(string line, int i,int row,int tab)
{
	string s="";
	//add characters to the literal until closing " is found
	while(i<line.length()&&line[i]!='"')
	{
		literal+=line[i];
		s+=line[i++];
	}
	if(line[i]=='"')
	{
		cout<<literal<<"\t:literal:\trow = "<<l_row<<" "<<"\tcolumn = "<<l_tab*(3)+l_col<<endl;
		cout<<line[i]<<"\t:punctuation:\trow = "<<row<<" "<<"\tcolumn = "<<tab*(3)+i+1<<endl;

		//when closing " is found turn off the literal flag and clear the literal
		literal_flag=0;
		literal.clear();
	}
	return s;
}

//check and return if there is a character constant in the given line at position i
string check_char_constant(string line, int i,int row,int tab)
{
	string s="";
	//add characters to the character constant until closing ' is found
	while(i<line.length()&&line[i]!='\'')
	{
		s+=line[i++];
	}
	if(line[i]=='\'')
	{
		cout<<literal<<"\t:char constant:\trow = "<<l_row<<" "<<"\tcolumn = "<<l_tab*(3)+l_col<<endl;
		cout<<line[i]<<"\t:punctuation:\trow = "<<row<<" "<<"\tcolumn = "<<tab*(3)+i+1<<endl;
	}
	return s;
}

//check each line from the input c file character wise
void check_line(string line,int row)
{
	int column,tab=0;
	for(int i=0;i<line.length();i++)
	{
		//if the literal flag is on then the current character is also part of that literal
		if(literal_flag)
		{
			string s=check_literal(line,i,row,tab);
			i+=s.length();
		}
		//if the character is space or tab ignore it
		else if(line[i]==' ')
			continue;
		else if(line[i]=='\t')
		{
			tab++;
			continue;
		}
		else
		{
			int x;
			column=i+1;
			if(check_punctuation(line,i)==1)
			{
				cout<<line[i]<<"\t:punctuation:\t";
				
			}
			else if((x=check_operator(line,i))!=-1)
			{

				cout<<line.substr(i,x)<<"\t:operator:\t";
				i+=(x-1);
			}
			else if((x=check_keyword(line,i))!=-1)
			{
				cout<<line.substr(i,x)<<"\t:keyword:\t";
				i+=(x-1);
			}
			else if((x=check_identifier(line,i))!=-1)
			{
				cout<<line.substr(i,x)<<"\t:identifier:\t";
				i+=(x-1);
			}
			else if((x=check_constant(line,i))!=-1)
			{
				cout<<line.substr(i,x)<<"\t:constant:\t";
				i+=(x-1);
			}
			else
				cout<<"Error at - ";
			cout<<"row = "<<row<<" "<<"\tcolumn = "<<tab*(3)+column<<endl; 
		}
		//if character is " then the following charcaters are literal
		if(line[i]=='"')
		{
			l_row=row;l_col=i+2;l_tab=tab;
			literal_flag=1;
			string s=check_literal(line,i+1,row,tab);
			i+=(s.length()+1);
		}
		//if character is ' then the following charcaters are character constant
		else if(line[i]=='\'')
		{
			string s=check_char_constant(line,i+1,row,tab);
			i+=(s.length()+1);
		} 
	}
}

//print the set of punctuations, operators and keywords
void print_set()
{
	set<string>::iterator it1,it2,it3;

	cout<<"set of punctuations: ";
	for(it1=p_set.begin();it1!=p_set.end();it1++)
		cout<<*it1<<" ";
	cout<<endl;

	cout<<"set of operators: ";
	for(it2=o_set.begin();it2!=o_set.end();it2++)
		cout<<*it2<<" ";
	cout<<endl;

	cout<<"set of keywords: ";
	for(it3=k_set.begin();it3!=k_set.end();it3++)
		cout<<*it3<<" ";
	cout<<endl;
}

//main function
int main(int argc, char const *argv[])
{
	if(argc!=2)	//if the number of arguments is not 2 then aborted
	{
		cout<<"Too Many Arguments"<<endl;
		return 0;
	}

	string c_file=argv[1];
	//print the file name passed via command line input
	cout<<c_file<<endl;
	ifstream myfile(c_file);	//open the input file
	string line;
	int row=0;

	get_punctuation_set("punctuations.txt");
	get_keyword_set("keywords.txt");
	get_operator_set("operators.txt");

	print_set();

	//read each line from the input file and parse it
	while (getline(myfile, line))
	{
		row++;
	    check_line(line,row);
	}
	myfile.close(); //close the file
	
	return 0;
}