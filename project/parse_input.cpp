#include "parse_input.h"

using namespace std;

string removeChar(string str, char rem)
{
    str.erase(remove(str.begin(), str.end(), rem), str.end()); 
    return str; 
}

int parse_vertex(string input, char *ip_cmd)
{
	int vtx;
	regex v_input("^[[:blank:]]*V{1}[[:blank:]]+([[:digit:]]+)[[:blank:]]*$");
	regex e_input("^[[:blank:]]*E{1}[[:blank:]]+\\{{1}([<>0-9, ]+)\\}{1}[[:blank:]]*$");
	regex s_input("^[[:blank:]]*s{1}[[:blank:]]+([[:digit:]]+)[[:blank:]]+([[:digit:]]+)[[:blank:]]*$");
	regex e_parsed("<(\\d+),(\\d+)>,{0,1}");
	smatch m;
	while(true)
 	{
 		if(regex_match(input,v_input))
 		{
 			//cout<<"valid vertex input"<<endl;
 			regex_search(input, m, v_input);
 			stringstream vertex(m[1].str());
 			vertex >> vtx;
 			//cout<<"Value of vtx = "<<vtx<<endl;
 			if(vtx>0)
 			{
 				*ip_cmd = 'E';
 				return vtx;
 			}
 			else
 			{
 				cout<<"Error: number of vertices should be more than 0"<<endl;
 				return 0;
 			}
 		}
 		else
 		{
 			cout<<"Error: Incorrect vertex input"<<endl;
 			return 0;
 		}
 	}

}

bool parse_edge(string input, vector< pair<int,int> >& edge_pair,int vtx, char *ip_cmd)
{
	regex v_input("^[[:blank:]]*V{1}[[:blank:]]+([[:digit:]]+)[[:blank:]]*$");
	regex e_input("^[[:blank:]]*E{1}[[:blank:]]+\\{{1}([<>0-9, ]+)\\}{1}[[:blank:]]*$");
	regex s_input("^[[:blank:]]*s{1}[[:blank:]]+([[:digit:]]+)[[:blank:]]+([[:digit:]]+)[[:blank:]]*$");
	regex e_parsed("<(\\d+),(\\d+)>,{0,1}");
	smatch m;
	string edge_string;
	while(true)
		{
 			if(regex_match(input,e_input))
 			{
 				//cout<<"entered"<<endl;
 				regex_search(input, m, e_input);
 				//cout<<m[1].str()<<endl;
 				edge_string = removeChar(m[1].str(),' ');
 				//cout<<"Removing spaces in edge string: "<<edge_string<<endl;
 				sregex_iterator it(edge_string.begin(),edge_string.end(),e_parsed);
				sregex_iterator it_end;
				int len_str1=edge_string.length();
				int len_str2=0;
				int edges=0;
				edge_pair.clear();
				int vtx1,vtx2;
 				while(it != it_end)
 				{
					std::smatch match = *it;
					string temps = match.str();
					regex_search(temps,m,e_parsed);
        			//cout<<"v1="<<m[1].str()<<" v2="<<m[2].str()<<endl;
        			stringstream vtx1str(m[1].str());
        			stringstream vtx2str(m[2].str());
        			vtx1str >> vtx1;
        			vtx2str >> vtx2;
        			if((vtx1!=vtx2)&&(vtx1>=0)&&(vtx2>=0)&&(vtx1<vtx)&&(vtx2<vtx))
        				edge_pair.push_back(make_pair(vtx1,vtx2));
        			else break;
        			len_str2+=(match.str()).length();
        			edges++;
        			//cout << match.str() << endl;
        			++it;
				}
				if(len_str1==len_str2)
				{
					*ip_cmd = 'V';
					return false;
				}

				else
				{
					cout<<"Error: Incorrect edge input"<<endl;
					return true;
				}
 			}
 			else if(regex_match(input,v_input))
 			{
 				*ip_cmd = 'V';
 				return true;
 			}
 			else
 			{
 				cout<<"Error: Incorrect edge input"<<endl;
 				return true;
 			}

 		}

}