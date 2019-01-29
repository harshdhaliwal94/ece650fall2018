#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid()
#include <signal.h>
#include <unistd.h>     // execv(), fork()
#include <cstdio>       //perror
#include <error.h>

#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <new>
//parsing utilities
#include "parse_input.h"
//Include minisat libraries
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

using namespace std;

#define NUM_THREADS 4

//Global variables
struct database{
   int vtx;
   std::vector< std::pair<int,int> > edge_list;
};

bool done_cnf =false;
bool done_approx1=false;
bool done_approx2=false;

std::vector<int> VClist; //CNF 
std::vector<int> VClistApprox1; //Approximate vertex cover 1
std::vector<int> VClistApprox2; //Approximate vertex cover 2

//funcion to print all the vertex cover lists
void printVC()
{
  //print the minimum sized Vertex Cover list
  if(VClist.empty()||VClistApprox1.empty()||VClistApprox2.empty())
    return;
  std::vector<int>::iterator it;
  std::vector<int>::iterator temp_it;
   
  std::cout<<"CNF-SAT-VC: ";
  for(it=VClist.begin();it!=VClist.end();++it)
  {
    temp_it = it;
    if(++temp_it==VClist.end())
      std::cout<<(*it);
    else
      std::cout<<(*it)<<",";
  }
  std::cout<<std::endl;

  std::cout<<"APPROX-VC-1: ";
  for(it=VClistApprox1.begin();it!=VClistApprox1.end();++it)
  {
    temp_it = it;
    if(++temp_it==VClistApprox1.end())
      std::cout<<(*it);
    else
      std::cout<<(*it)<<",";
  }
  std::cout<<std::endl;

  std::cout<<"APPROX-VC-2: ";
  for(it=VClistApprox2.begin();it!=VClistApprox2.end();++it)
  {
    temp_it = it;
    if(++temp_it==VClistApprox2.end())
      std::cout<<(*it);
    else
      std::cout<<(*it)<<",";
  }
  std::cout<<std::endl;

  return;
}

//IO thread reads V,E and prints vertex cover lists
void *IO_Thread(void *t) {
   //char* success="successfull";
   struct database *graph = (struct database *)t;
   char ip_cmd='V';
   std::string input;
   bool first_input=true;
   bool wrong_cmd=false;
   //output
   while(true)
   {
      if(done_cnf&&done_approx1&&done_approx2)
      {
         done_cnf=false;
         done_approx1=false;
         done_approx2=false;
         printVC();
         break;
      }
   }

   //input
   while(true)
   {
      switch(ip_cmd){
         case 'V':
            if(first_input)
            {
               getline (std::cin, input);
               if(!std::cin) exit(0);
               first_input=false;
            }
            graph->vtx = parse_vertex(input,&ip_cmd);
            if(graph->vtx<=0)
               first_input=true;
            break;
         case 'E':
            getline (std::cin, input);
            if(!std::cin) exit(0);
            wrong_cmd=parse_edge(input,graph->edge_list,graph->vtx,&ip_cmd);
            if(wrong_cmd==false)
            {
               first_input=true;
               pthread_exit(NULL);
            }
            break;
         }
   }
   pthread_exit(NULL);
}

//Thread to compute vertex cover using CNF SAT solver minisat
void *VtxCoverCNF(void *t)
{
    struct database * graph = (struct database *)t;
    if(graph->edge_list.empty())
    {
      done_cnf=true;
      pthread_exit(NULL);
    }
    int n = graph->vtx;
    std::vector< std::pair<int,int> > &edge = graph->edge_list;
    //allocate solver object on heap using smart unique pointer so that we can reset it later is needed
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    //Variable declaration
    int k=0; //k is the size of vertex cover
    int lo=0;
    int hi=n;
    bool result = false;
    //Do a binary search for k
    while(lo<hi)
    {
      k = (lo+hi)/2;
      //create variables and initialize all literals lit(n)
      std::vector< std::vector< Minisat::Lit > > lit(n);
      for(int i=0;i<n;++i)
      {
         for(int j=0;j<k;++j)
         {
            Minisat::Lit tempLit = Minisat::mkLit(solver->newVar());
            lit[i].push_back(tempLit);
         }
      }
      //Rule 1: At least one vertex is the ith vertex in the vertex list
      for(int i=0;i<k;++i)
      {
         Minisat::vec<Minisat::Lit> tempVec;
         for(int j=0;j<n;++j)
            tempVec.push(lit[j][i]);
         solver->addClause(tempVec);
         tempVec.clear();
      }
      //Rule 2: No one vertex ca appear twice in the vertex cover list
      for(int i=0;i<n;++i)
      {
         for(int j=0;j<k-1;++j)
         {
            for(int l=j+1;l<k;++l)
               solver->addClause(~lit[i][j],~lit[i][l]);
         }
      }
      //Rule 3: No more than one vertex appear in ith position of the vertex cover list
      for(int i=0;i<k;++i)
      {
         for(int j=0;j<n-1;++j)
         {
            for(int l=j+1;l<n;++l)
               solver->addClause(~lit[j][i],~lit[l][i]);
         }
      }
      //Rule 4: Every edge is incident on atleast one vertex in the vertex cover
      std::vector< std::pair<int,int> >::iterator itr;
      for(itr=edge.begin();itr!=edge.end();++itr)
      {
         Minisat::vec<Minisat::Lit> tempVec;
         for(int i=0;i<k;++i)
         {
            tempVec.push(lit[(*itr).first][i]);
            tempVec.push(lit[(*itr).second][i]);
         }
         solver->addClause(tempVec);
         tempVec.clear();
      }
      // if UNSAt => result=0 else if SAT => result=1
      result = solver->solve();
      if(result==1)
         hi = k;
      else if(result==0)
         lo = k+1;
      lit.clear();
      solver.reset(new Minisat::Solver());
    }
    k = hi;
    //create variables and initialize all literals lit(n)
    std::vector< std::vector< Minisat::Lit > > lit(n);
    for(int i=0;i<n;++i)
    {
      for(int j=0;j<k;++j)
      {
         Minisat::Lit tempLit = Minisat::mkLit(solver->newVar());
         lit[i].push_back(tempLit);
      }
    }
    //Rule 1: At least one vertex is the ith vertex in the vertex list
    for(int i=0;i<k;++i)
    {
      Minisat::vec<Minisat::Lit> tempVec;
      for(int j=0;j<n;++j)
         tempVec.push(lit[j][i]);
      solver->addClause(tempVec);
      tempVec.clear();
    }
    //Rule 2: No one vertex ca appear twice in the vertex cover list
    for(int i=0;i<n;++i)
    {
      for(int j=0;j<k-1;++j)
      {
         for(int l=j+1;l<k;++l)
            solver->addClause(~lit[i][j],~lit[i][l]);
      }
    }
    //Rule 3: No more than one vertex appear in ith position of the vertex cover list
    for(int i=0;i<k;++i)
    {
      for(int j=0;j<n-1;++j)
      {
         for(int l=j+1;l<n;++l)
            solver->addClause(~lit[j][i],~lit[l][i]);
      }
    }
    //Rule 4: Every edge is incident on atleast one vertex in the vertex cover
   std::vector< std::pair<int,int> >::iterator itr;
   for(itr=edge.begin();itr!=edge.end();++itr)
   {
      Minisat::vec<Minisat::Lit> tempVec;
      for(int i=0;i<k;++i)
      {
         tempVec.push(lit[(*itr).first][i]);
         tempVec.push(lit[(*itr).second][i]);
      }
      solver->addClause(tempVec);
      tempVec.clear();
   }
   // if UNSAt => result=0 else if SAT => result=1
   result = solver->solve();
   // Get the vertex cover list
   VClist.clear();
   for(int i=0; i<n; ++i)
    {
        for(int j=0; j<k; ++j)
        {
            if(solver->modelValue(lit[i][j]) == Minisat::l_True)    
                VClist.push_back(i);
        }
    }
    //sort
    //std::sort(begin(VClist),end(VClist),[](int const &t1, int const &t2) { return t1<t2;});
    std::sort(VClist.begin(), VClist.end(), std::less<int>());
    lit.clear();
    solver.reset (new Minisat::Solver());
    done_cnf=true;
    pthread_exit(NULL);
}

//Thread to compute approximate vertex cover using method 1
void *VtxCoverApprox1(void *t)
{
  struct database * graph_input = (struct database *)t;
  if(graph_input->edge_list.empty())
  {
    done_approx1=true;
    pthread_exit(NULL);
  }
  int *degree = new (std::nothrow) int[graph_input->vtx];
  if(!degree)
    std::cerr<<"Error: new failed to allocate array degree"<<std::endl;
  for(int i=0;i<graph_input->vtx;++i)
    degree[i]=0; 
  int max_deg_vtx=0;
  int max_deg=1;
  VClistApprox1.clear();
  //create a local copy of the graph structure
  struct database graph;
  graph.vtx = graph_input->vtx;
  std::vector< std::pair<int,int> >::iterator itr;
  for(itr=graph_input->edge_list.begin();itr!=graph_input->edge_list.end();++itr)
    {
      graph.edge_list.push_back(*itr);
      degree[(*itr).first]++;
      degree[(*itr).second]++;
      if(max_deg<=degree[(*itr).first])
      {
        max_deg=degree[(*itr).first];
        max_deg_vtx=(*itr).first;
      }
      if(max_deg<=degree[(*itr).second])
      {
        max_deg=degree[(*itr).second];
        max_deg_vtx=(*itr).second;
      }
    }
    while(!(graph.edge_list.empty()))
    {
      //Add the vertex with max degree to vc list
      VClistApprox1.push_back(max_deg_vtx);
      degree[max_deg_vtx]=0;
      max_deg=1;
      //delete all edges incident on it
      for(itr=graph.edge_list.begin();itr!=graph.edge_list.end();)
      {
        if((*itr).first==max_deg_vtx)
        {
          degree[(*itr).second]--;
          graph.edge_list.erase(itr);
        }
        else if((*itr).second==max_deg_vtx)
        {
          degree[(*itr).first]--;
          graph.edge_list.erase(itr);
        }
        else
          ++itr;
      }
      //find new maximum degree vertex
      for(itr=graph.edge_list.begin();itr!=graph.edge_list.end();++itr)
      {
        if(max_deg<=degree[(*itr).first])
        {
          max_deg=degree[(*itr).first];
          max_deg_vtx= (*itr).first;
        }
        if(max_deg<=degree[(*itr).second])
        {
          max_deg=degree[(*itr).second];
          max_deg_vtx= (*itr).second;
        }
      }

    }
  std::sort(VClistApprox1.begin(), VClistApprox1.end(), std::less<int>());
  done_approx1=true;
  pthread_exit(NULL);
}

//Thread to compute approximate vertex cover using method 2
void *VtxCoverApprox2(void *t)
{
  struct database * graph_input = (struct database *)t;
  if(graph_input->edge_list.empty())
  {
    done_approx2=true;
    pthread_exit(NULL);
  }
  //create a local copy of the graph structure
  struct database graph;
  graph.vtx = graph_input->vtx;
  std::vector< std::pair<int,int> >::iterator itr;
  for(itr=graph_input->edge_list.begin();itr!=graph_input->edge_list.end();++itr)
    graph.edge_list.push_back(*itr);
  VClistApprox2.clear();
  while(!(graph.edge_list.empty()))
  {
    //pick an edge add both vtx to vc list
    std::pair<int,int> edge = graph.edge_list[0];
    VClistApprox2.push_back(edge.first);
    VClistApprox2.push_back(edge.second);
    //remove all edges incident on these vertices
    for(itr=graph.edge_list.begin();itr!=graph.edge_list.end();)
    {
      if((*itr).first==edge.first||(*itr).first==edge.second)
        graph.edge_list.erase(itr);
      else if((*itr).second==edge.first||(*itr).second==edge.second)
        graph.edge_list.erase(itr);
      else
        itr++;
    } 
  }
  std::sort(VClistApprox2.begin(), VClistApprox2.end(), std::less<int>());
  done_approx2=true;
  pthread_exit(NULL);
}

int main () 
{
    int rc;
    int i;
    struct database graph;
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;
    while(true)
    {
      // Initialize and set thread joinable
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
 
      rc = pthread_create(&threads[0], &attr, VtxCoverCNF, (void *)&graph );
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
      rc = pthread_create(&threads[1], &attr, VtxCoverApprox1, (void *)&graph );
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
      rc = pthread_create(&threads[2], &attr, VtxCoverApprox2, (void *)&graph );
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
      rc = pthread_create(&threads[3], &attr, IO_Thread, (void *)&graph );
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }

      // free attribute and wait for the other threads
      pthread_attr_destroy(&attr);
      for( i = 0; i < NUM_THREADS; i++ ) {
         rc = pthread_join(threads[i], &status);
         if (rc) {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
         }
      }
    }
  
  return 0;
}