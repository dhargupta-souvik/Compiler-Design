#include <bits/stdc++.h>

using namespace std;

int operands=0;
string re="";
vector<vector<int> > folpos;
vector<char> symbolmap;

char st_sym='A';

bool is_op(char x)
{
	if(x=='|'||x=='.'||x=='*'||x=='('||x==')')
		return true;
	return false;
}

typedef struct dfa
{
	set<int> state;
	char state_symbol;
	int check;
	vector<pair<char,set<int> > >next_state;
}dfa;

vector<dfa*> new_dfa;

typedef struct tree
{
	bool op;
	char val;
	int label;
	bool nullable;
	vector<int> fpos;
	vector<int> lpos;
	// vector<int> folpos;
	struct tree* left;
	struct tree* right;
}tree;

void print_tree(tree* t)
{
	if(t)
	{
		cout<<t->val<<"\t";

		if(t->left)
			cout<<t->left->val<<"\t";
		else cout<<"NA\t";
		if(t->right)
			cout<<t->right->val<<"\t{";
		else cout<<"NA\t{";

		for(int i=0;i<t->fpos.size();i++)
		{
			cout<<t->fpos[i];
			if(i<t->fpos.size()-1)
				cout<<",";
			else cout<<"}";
		}
		cout<<"\t\t";
		cout<<"{";
		for(int i=0;i<t->lpos.size();i++)
		{
			cout<<t->lpos[i];
			if(i<t->lpos.size()-1)
				cout<<",";
			else cout<<"}";
		}
		cout<<"\t\t"<<t->nullable<<endl;
		print_tree(t->left);
		print_tree(t->right);
	}
	return;
}

tree* generate_tree()
{
	tree* t=new tree();
	char c;
	int len=re.length();
	c=re[len-1];
	
	t->val=re[len-1];
	t->op=is_op(re[len-1]);
	if(!t->op)
	{
		re.erase(len-1,1);
		t->left=nullptr;
		t->right=nullptr;
		t->label=operands--;

		symbolmap[t->label]=t->val;
	}
	else
	{
		t->label=-1;
		if(c=='*')
		{
			re.erase(len-1,1);
			t->left=generate_tree();
			t->right=nullptr;
		}
		else
		{
			re.erase(len-1,1);
			t->right=generate_tree();
			t->left=generate_tree();
		}
	}
	return t;
}

vector<int> vunion(vector<int> v1,vector<int> v2)
{
	vector<int> v;
	for(int i=0;i<v1.size();i++)
	{
		v.push_back(v1[i]);
	}
	for(int i=0;i<v2.size();i++)
	{
		int j;
		for(j=0;j<v1.size();j++)
		{
			if(v1[j]==v2[i])
				break;
		}
		if(j==v1.size())
		{
			v.push_back(v2[i]);
		}
	}

	return v;
}

void compute_fpos_lpos(tree* t)
{
	
	if(!t)
		return;
	if(!is_op(t->val))
	{
		t->fpos.push_back(t->label);
		t->lpos.push_back(t->label);
		t->nullable=false;
	}
	else
	{
		compute_fpos_lpos(t->left);
		compute_fpos_lpos(t->right);
		if(t->val=='|')
		{
			t->fpos=vunion(t->left->fpos,t->right->fpos);
			t->lpos=vunion(t->left->lpos,t->right->lpos);
			t->nullable=t->left->nullable|t->right->nullable;
		}
		else if(t->val=='.')
		{
			if(t->left->nullable)
				t->fpos=vunion(t->left->fpos,t->right->fpos);
			else
				t->fpos=t->left->fpos;
			if(t->right->nullable)
				t->lpos=vunion(t->left->lpos,t->right->lpos);
			else
				t->lpos=t->right->lpos;
			t->nullable=t->left->nullable&t->right->nullable;
		}
		else
		{
			t->fpos=t->left->fpos;
			t->lpos=t->left->lpos;
			t->nullable=true;
		}
	}
}

void compute_follow_pos(tree* t)
{
	if(!t)
		return;
	if(t->val=='.')
	{
		for(int i=0;i<t->left->lpos.size();i++)
		{
			for(int j=0;j<t->right->fpos.size();j++)
			{
				folpos[t->left->lpos[i]-1].push_back(t->right->fpos[j]);
			}
		}
	}
	else if(t->val=='*')
	{
		for(int i=0;i<t->lpos.size();i++)
		{
			for(int j=0;j<t->fpos.size();j++)
			{
				folpos[t->lpos[i]-1].push_back(t->fpos[j]);
			}
		}
	}
	compute_follow_pos(t->left);
	compute_follow_pos(t->right);
}

void print_follow_pos()
{
	cout<<endl<<endl<<"Node\t\tFollowpos"<<endl;
	for(int i=0;i<folpos.size();i++)
	{
		cout<<i+1<<"\t\t{";
		if(folpos[i].size()==0)
			cout<<"}";
		for(int j=0;j<folpos[i].size();j++)
		{
			cout<<folpos[i][j];
			if(j==folpos[i].size()-1)
				cout<<"}";
			else cout<<",";
		}
		cout<<endl;
	}
}

string infix2postfix(string s)
{
	string ns="";
	s="("+s+")";
	stack<char> st;
	for(int i=0;i<s.length();i++)
	{
		if(!is_op(s[i]))
		{
			ns+=s[i];
			operands++;
		}
		else if(s[i]=='*')
		{
			ns+=s[i];
		}
		else if(s[i]=='(')
		{
			st.push(s[i]);
		}
		else if(s[i]==')')
		{
			while(st.top()!='(')
			{
				ns+=st.top();
				st.pop();
			}
			st.pop();
		}
		else
		{
			if(!st.empty()&&st.top()!='(')
			{
				ns+=st.top();
				st.pop();
			}
			st.push(s[i]);
		}
	}
	return ns;
}

bool check_marked(set<int> s)
{
	for(int i=0;i<new_dfa.size();i++)
	{
		if(s==new_dfa[i]->state)
			return true;
	}
	return false;
}

void generate_dfa(dfa* d)
{

	for(auto it=d->state.begin();it!=d->state.end();it++)
	{
		if(symbolmap[*it]=='#')
			continue;		

		int i;
		for(i=0;i<d->next_state.size();i++)
		{

			if(symbolmap[*it]==d->next_state[i].first)
			{
				for(int j=0;j<folpos[*it-1].size();j++)
					d->next_state[i].second.insert(folpos[*it-1][j]);
				break;
			}
		}
		if(i==d->next_state.size())
		{
			set<int> s;
			for(int j=0;j<folpos[*it-1].size();j++)
				s.insert(folpos[*it-1][j]);
			if(s.empty())
				continue;
			d->next_state.push_back(make_pair(symbolmap[*it],s));
		}
	}

	new_dfa.push_back(d);
	for(int l=0;l<d->next_state.size();l++)
	{
		if(!check_marked(d->next_state[l].second))
		{
			dfa* d1=new dfa();
			d1->state=d->next_state[l].second;
			d1->check=0;
			d1->state_symbol=st_sym++;

			generate_dfa(d1);
		}
	}
		
}

void print_dfa()
{
	cout<<"\n\nThe generated dfa is : \n";
	for(int i=0;i<new_dfa.size();i++)
	{
		
		for(auto it=new_dfa[i]->state.begin();it!=new_dfa[i]->state.end();it++)
		{
			if(it==new_dfa[i]->state.begin())
				cout<<"{";
			else cout<<",";
			cout<<*it;
		}
		cout<<"}";

		for(int j=0;j<new_dfa[i]->next_state.size();j++)
		{
			cout<<"\t\t"<<new_dfa[i]->next_state[j].first<<"-> ";

			for(auto it=new_dfa[i]->next_state[j].second.begin();it!=new_dfa[i]->next_state[j].second.end();it++)
			{
				if(it==new_dfa[i]->next_state[j].second.begin())
					cout<<"{";
				else cout<<",";
				cout<<*it;
			}
			cout<<"}";
		}

		cout<<endl;
	}
}

int main()
{
	cout<<"Enter the Regular Expression : ";
	string input;
	getline(cin,input);
	re+=infix2postfix(input+".#");
	int op=operands;

	cout<<"The Regular Expression in Postfix format is : "<<re<<endl;

	for(int i=0;i<=op;i++)
	{
		symbolmap.push_back('$');
	}

	tree* st=generate_tree();
	

	compute_fpos_lpos(st);
	cout<<"Node\tLeft\tRight\tFirstpos\tLastpos\t\tNullable"<<endl;
	print_tree(st);

	
	folpos.clear();
	for(int i=0;i<op;i++)
	{
		vector<int> v;
		folpos.push_back(v);
	}
	compute_follow_pos(st);
	print_follow_pos();

	
	dfa* d=new dfa();
	for(int i=0;i<st->fpos.size();i++)
	{
		d->state.insert(st->fpos[i]);
	}
	d->check=0;
	d->state_symbol=st_sym++;

	generate_dfa(d);

	print_dfa();
}