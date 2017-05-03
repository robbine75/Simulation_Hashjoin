// SimQP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dbmsproj.h"
#include "semihashjoin.h"
#include <string>
#include <sstream>
#include <stdio.h>
#define GET_VARIABLE_NAME(Variable) (#Variable)
/***Using vectors represent disk to hold records block*****/

//NYC - This site has a copy of branch, account and depositor tables.
std::vector<block_t<account_t>> account_NYC;
std::vector<block_t<branch_t>> branch_NYC;
std::vector<block_t<depositor_t>> depositor_NYC;

//SFO - This site has complete branch table and a fragment of account table.
//Account table will have only records that are local to SFO.
std::vector<block_t<branch_t>> branch_SFO;
std::vector<block_t<account_t>> frag_account_SFO;

//OMA - This site has fragments of Account and Depositor tables.
//These tables store records that are local to OMA.
std::vector<block_t<account_t>> frag_account_OMA;
std::vector<block_t<depositor_t>> frag_depositor_OMA;

//HOU (Houston) ¨C This site has a complete copy of customer table.
std::vector<block_t<customer_t>> customer_HOU;


//unsigned int MAX_RECORDS_PER_BLOCK=7;


int main(int argc, char* argv[])
{
   	
	//initialize 
	char* filename;
	
	//fill account table at NYC
	filename = "Account_NYC.txt";
	fillTable<account_t>(filename, account_NYC, 6);

	//fill branch table at NYC
	filename = "Branch_NYC.txt";
	fillTable<branch_t>(filename, branch_NYC, 1);
	
	//fill depositor table at NYC
	filename = "Depositor_NYC.txt";
	fillTable<depositor_t>(filename, depositor_NYC, 4);
	
	//fill customer table at HOU
	filename= "Customer_HOU.txt";
	fillTable<customer_t>(filename, customer_HOU, 8);
	
	//fill fragment of account table at SFO
	filename = "Frag_Account_SFO.txt";
	fillTable<account_t>(filename, frag_account_SFO, 2);
	
	//fill fragment of depositor table at OMA
	filename = "Frag_Depositor_OMA.txt";
	fillTable<depositor_t>(filename, frag_depositor_OMA, 2);

	//fill fragment of account table at OMA
	filename = "Frag_Account_OMA.txt";
	//fillTable<account_t>(filename, frag_account_OMA, 3);
	//printf("All tables are filled.\n");
	//printf("============================================================\n");
	std::istringstream ss(argv[1]);
	int query;
	if (!(ss >> query)) std::cerr << "Invalid number" << argv[1] << std::endl;

	std::vector<std::string> command;
	std::vector<std::string> parameters;
	std::string require_site;
	std::string current_site;

	
	switch (query)
	{
	case 1:
			{
				std::vector<block_t<customer_t>> out;
				printf("Query 1: \n");
				printf("Require site: NYC\n");
				printf("Statement: Select* from customer |X depositor:\n");
				printf("Processing:\n");
				printf("The customer table is shipped from HOU to NYC site. \n");
								
				printf("\n");
				SemiHashJoin<customer_t,depositor_t>(customer_HOU,depositor_NYC,"customer_name", out);
				printf("The customer table are SemiHashJoined with depositor table at NYC site. \n");
				printf("Project all fields in customer table.\n");
				printf("\n");
				printf("Output: \n");
				printf("===========================================\n");
				printf("Name	Street	City \n");
				printf("===========================================\n");
				unsigned int count = 0;
				for each ( auto block in out)
				{
		
					for each (auto record in block.entries)
						{
							record.project(3, "customer_name", "customer_city","customer_street");
							count++;
						}

				}
				printf("===========================================\n");
				printf("The query output %d records.\n",count);
				break;
			}
	case 2:
	       {
	
		      printf("Query 2: \n");
		      printf("Require site: SFO\n");
			  printf("Statement: Select name, balance from depositor |X| account where branch_name='Chinatown':\n");
			  printf("Processing:\n");
		 			  
			  printf("3) Ship the results from NYC to SFO and then do projection. \n");
		      printf("\n");
			   std::vector<block_t<account_t>> table_selec;
              printf("Select the records with branch_name='Chinatown' using the fragment of account table at SFO.\n");
			   select(account_NYC, table_selec, "branch_name", "Chinatown");

			   if (table_selec.size() == 0)
			   { 
				   printf("no record is selected according to the condition.\n"); 
				   break; 
			   }
			   else
			   {
				   unsigned int rec_selected = (table_selec.size() - 1)*table_selec[0].maxRecords + table_selec[table_selec.size()-1].nreserved;
				   //printf("%d records are selected.\n", rec_selected);
			   }
			   printf("Ship the selection results from SFO to HOU  and hash joined with depositor table. \n");
			   std::vector<join_t<depositor_t,account_t>> joinout;
	           HashJoin<depositor_t,account_t>(depositor_NYC, table_selec,"account_number", joinout);
			   printf("The join results at HOU is shipped back to SFO. \n");
			   printf("\n");
			   printf("Output: \n");
			   printf("===========================================\n");
			   printf("Name	Balance\n");
			   printf("===========================================\n");
			   unsigned int count=0;
			   for each (auto var in joinout)
	          {
		               var.display(2,"customer_name","balance");
					   count++;
	          }	
			   printf("===========================================\n");
			   printf("The query output %d records.\n", count);
			   break;
	       }
	case 3:
	{

		printf("Query 3: \n");
		printf("Require site: SFO\n");
		printf("Statement: Select street, city from customer|X (depositor |X account where account_number='A10352'):\n");
		printf("Processing:\n");
		
		printf("\n");
		std::vector<block_t<account_t>> table_selec;
		
		printf("Search the fragment of account table at SFO to check if there have records with account_number='A10352' \n");
        select(frag_account_SFO, table_selec, "account_number", "A10352");
		if (table_selec.size() == 0)
		{
			printf("no record is found according to the condition.\n");
			select(account_NYC, table_selec, "account_number", "A10352");
			printf("Search the account table at NYC.\n");
		}
		
		if (table_selec.size() == 0)
		{
			printf("no record is selected according to the condition.\n");
			break;
		}
		else
		{
			
			unsigned int rec_selected = (table_selec.size() - 1)*table_selec[0].maxRecords + table_selec[table_selec.size() - 1].nreserved;
			//printf("%d records are selected.\n", rec_selected);
		}

		std::vector<block_t<depositor_t>> semi_out;
		printf("The depositor table is semi-joined with intermediate result from select operation at NYC.\n");
		SemiHashJoin<depositor_t,account_t>(depositor_NYC,table_selec,"account_number", semi_out);

		printf("The intermedate results from semijoin are shipped from NYC to HOU. \n");
		printf("The customer table at HOU is hash joined with the intermedate results. \n");
		std::vector<join_t<customer_t, depositor_t>> joinout;
		HashJoin<customer_t, depositor_t>(customer_HOU, semi_out, "customer_name", joinout);
		
		printf("The final result is shipped from HOU to the SFO. \n");

		printf("Output: \n");
		printf("===========================================\n");
		printf("Street		City\n");
		printf("===========================================\n");
		
		unsigned int count=0;
		for each (auto var in joinout)
		{
			var.display(2, "customer_street", "customer_city");
			count++;
		}
		printf("===========================================\n");
		printf("The query output %d records.\n", count);
		break;
	}
	case 4:
	{

		printf("Query 4: \n");
		printf("Require site: NYC\n");
		printf("Statement: Select account_number, balance, branch_name branch_city from branch|X| acocunt where account_number='A10352'):\n");
		printf("Processing:\n");
		
		std::vector<block_t<account_t>> table_selec;
		printf("Select the record from the account table at NYC where account_number='A10352' \n");
		select(account_NYC, table_selec, "account_number", "A10352");
		if (table_selec.size() == 0)
		{
			//printf("no record is selected according to the condition.\n");
			break;
		}
		else
		{
			unsigned int rec_selected = (table_selec.size() - 1)*table_selec[0].maxRecords + table_selec[table_selec.size() - 1].nreserved;
			//printf("%d records are selected.\n", rec_selected);
		}

		std::vector<join_t<branch_t, account_t>> joinout;
		printf("The branch table is hash joined with intermediate result from select operation at NYC.\n");
		HashJoin<branch_t, account_t>(branch_NYC, table_selec, "branch_name", joinout);
		printf("===========================================\n");

		printf("Output: \n");
		printf("===========================================\n");
		printf("account_nmber	balance	branch_name	branch_city\n");
		printf("===========================================\n");
		unsigned int count = 0;
		for each (auto var in joinout)
		{
			var.display(4, "account_number", "balance", "branch_name", "branch_city");
			count++;
		}
		printf("===========================================\n");
		printf("The query output %d records.\n", count);
		break;
	}
	default:
		break;
	}

	printf("\n");

}

