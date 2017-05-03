/*
* DBMS Implementation
*/

#ifndef _DBMSPROJ_H
#define _DBMSPROJ_H

#include "stdafx.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdarg>
//#include "bufferOps.h"

#define TUPLES_PER_ACCOUNT_BLOCK 10
#define TUPLES_PER_BRANCH_BLOCK 7
#define TUPLES_PER_CUSTOMER_BLOCK 8
#define TUPLES_PER_DEPOSITOR_BLOCK 15

#define NUM_BLOCKS_ACCOUNT 6
#define NUM_BLOCKS_BRANCH 1
#define NUM_BLOCKS_CUSTOMER 8
#define NUM_BLOCKS_DEPOSITOR 4

#define MAX_RECORDS_PER_BUCKET 20


#define MAX_MEMORY_BLOCKS 5

//definition of record
//structure of records in account table;
enum field_code {e_account_number,e_branch_name,e_balance,e_branch_city,e_assets,e_customer_name,e_customer_street,e_customer_city};
inline field_code getfield(std::string const& field)
{
	if (field == "account_number") return e_account_number;
	if (field == "branch_name") return e_branch_name;
	if (field == "balance") return e_balance;
	if (field == "branch_city") return e_branch_city;
	if (field == "assets") return e_assets;
	if (field == "customer_name") return e_customer_name;
	if (field == "customer_street") return e_customer_street;
	if (field == "customer_city") return e_customer_city;
};
typedef struct {
	std::string account_number;
	std::string branch_name;
	int balance = 0;
	bool valid=false; //if set, then the record is valid
	void setCol(std::string &val1, std::string &val2, std::string &val3)
	{
		account_number = val1;
		branch_name = val2;
		balance = std::stoi(val3,0);
		valid = true;
	}
	std::string getCol(std::string const& field)
	{
		field_code fields = getfield(field);
		
		switch (fields)
		{
		case e_account_number: return account_number; break;
		case e_branch_name: return branch_name; break;
		case e_balance: return std::to_string(balance); break;
		default: return (" "); break;
		}
	}

	void display() {
		std::cout << account_number << "	" << branch_name << "	" << balance << std::endl;
	}
	void project(int num, const char* field, ...)
	{
		std::string fields(field);
		if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";
		va_list arguments;
		va_start(arguments, field);
		for (size_t i = 0; i < num - 1; i++)
		{
			std::string fields(va_arg(arguments, char*));
			if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";
		}
		std::cout << std::endl;
		va_end(arguments);
	}

}account_t;

//structure of records in branch table;
typedef struct {
	std::string branch_name;
	std::string branch_city;
	int assets = 0;
	 bool valid=false; //if set, then the record is valid
	std::string getCol(std::string const& field)
	{
		field_code fields = getfield(field);
	  
		switch (fields)
		{
		case e_branch_name: return branch_name; break;
		case e_branch_city: return branch_city; break;
		//case 3: return assets; break;
		default:return (" ");
			break;
		}
	}
	void setCol(std::string &val1, std::string &val2, std::string &val3)
	{
		branch_name = val1;
		branch_city = val2;
		assets = std::stoi(val3, 0);
		valid = true;
	}
	void display() {
		std::cout << branch_name << "	" << branch_city << "	" << assets << std::endl;
	}
	void project(int num, const char* field, ...)
	{
		std::string fields(field);
		if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";
		va_list arguments;
		va_start(arguments, field);
		for (size_t i = 0; i < num - 1; i++)
		{
			std::string fields(va_arg(arguments, char*));
			if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";
		}
		std::cout << std::endl;
		va_end(arguments);
	}
}branch_t;

//structure of records in customer table;
typedef struct {
	std::string customer_name;
	std::string customer_street;
	std::string customer_city;
	bool valid=false; //if set, then the record is valid
	void setCol(std::string &val1, std::string &val2, std::string &val3)
	{
		customer_name = val1;
		customer_street = val2;
		customer_city = val3;
		valid = true;
	}
	std::string getCol(std::string const& field)
	{
		field_code fields = getfield(field);
		switch (fields)
		{
		case e_customer_name: return customer_name; break;
		case e_customer_street: return customer_street; break;
		case e_customer_city: return customer_city; break;
		default:return (" ");
			break;
		}
	}
	void display() {
		std::cout << customer_name << "	" << customer_street << "	" << customer_city << std::endl;
	}
	void project(int num, const char* field, ...)
	{
		std::string fields(field);
		if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";
		va_list arguments;
		va_start(arguments, field);
		for (size_t i = 0; i < num - 1; i++)
		{
			std::string fields(va_arg(arguments, char*));
			if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";
		}
		std::cout << std::endl;
		va_end(arguments);
	}
}customer_t;

//structure of records in depositor table;
typedef struct {
	std::string customer_name;
	std::string account_number;
	bool valid=false; //if set, then the record is valid
	void setCol(std::string &val1, std::string &val2, std::string &val3)
	{
		customer_name = val1;
		account_number = val2;
		valid = true;
	}

	std::string getCol(std::string const& field)
	{
		field_code fields = getfield(field);
		switch (fields)
		{
		case e_customer_name: return customer_name; break;
		case e_account_number: return account_number; break;
		default: return (" "); break;
		}
	}
	void display() {
		std::cout << account_number << "	" << customer_name<< std::endl;
	}
	void project(int num, const char* field, ...)
	{
		std::string fields(field);
		if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";	
		va_list arguments;
		va_start(arguments, field);
		for (size_t i = 0; i < num - 1; i++)
		{
			std::string fields(va_arg(arguments, char*));
			if (getCol(fields) != (" ")) std::cout << getCol(fields) << "	";		
		}
		std::cout << std::endl;
		va_end(arguments);
	}
}depositor_t;

template<typename T> struct block_t {
	unsigned int blockid;
	unsigned int nreserved;
	unsigned int maxRecords;
	bool valid;
	std::vector<T> entries;
	
	block_t()
	{
		blockid = 0;
		nreserved = 0;
		valid = false;
		if (std::is_same<T, account_t>::value) maxRecords = 10;
		if (std::is_same<T, branch_t>::value)  maxRecords = 7;
		if (std::is_same<T, customer_t>::value)  maxRecords = 8;
		if (std::is_same<T, depositor_t>::value)  maxRecords = 15;
		//entries.reserve(maxRecords);
	}

	void printrecord() 
	{
		for (unsigned int i = 0; i < entries.size(); i++)
		{
			entries[i].display();
		}
	}
	
};




template<typename T1, typename T2> struct join_t {
	T1 rec1;
	T2 rec2;
	void display(int num, std::string field, ...)
	{
		std::string fields(field);
		if (rec1.getCol(fields) != (" ")) std::cout << rec1.getCol(fields) << "	";
		if (rec2.getCol(fields) != (" ")) std::cout << rec2.getCol(fields) << "	";
		va_list arguments;
		va_start (arguments, field);
		for (size_t i = 0; i < num-1; i++)
		{
			std::string fields(va_arg(arguments, char*));
			if (rec1.getCol(fields) != (" ")) std::cout << rec1.getCol(fields) <<"	";
			if (rec2.getCol(fields) != (" ") ) std::cout << rec2.getCol(fields)<<"	";
		}
		std::cout << std::endl;
		va_end(arguments);
	}
};



/* ----------------------------------------------------------------------------------------------------------------------
  Functions
  ----------------------------------------------------------------------------------------------------------------------
 */

template<typename T> void fillTable(char* filename, std::vector<block_t<T>> &table, unsigned int numblocks, int degbugmode = 0) {

	std::ifstream file(filename);

	int rec_counts = 0;
	int block_counts = 0;

	for (unsigned int i = 0; i < numblocks; i++)
	{
		std::string   line;
		unsigned int index = 0;
		block_t<T> block;

		while ((index<block.maxRecords) && (std::getline(file, line)))
		{
			std::string   val1;
			std::string   val2;
			std::string   val3;
			T rec;
			std::stringstream   linestream(line);
			std::string         data;
			std::getline(linestream, val1, '\t');
			std::getline(linestream, val2, '\t');
			std::getline(linestream, val3, '\t');

			rec.setCol(val1, val2, val3);
			rec.valid = true;
			block.entries.push_back(rec);
			rec_counts++;
			index++;
		}
		block.blockid = i;
		block.nreserved = index;
		block.valid = true;
		table.push_back(block);
		block_counts++;
	}

	if (degbugmode !=0)
	{
	
	printf("%s Table in NYC site has %d records in %d blocks.\n", filename,rec_counts, block_counts);
	for each (block_t<T> block in table)
	{
		block.printrecord();
		printf("\n");
	}
	printf("\n");
	}
}
template <typename T> void select(std::vector<block_t<T>> &in, std::vector<block_t<T>> &out, std::string const & field, std::string const & val,int debugmod=0) 
{
	printf("Start processing selection.... \n");
	block_t<T> buffer[MAX_MEMORY_BLOCKS];
	//figure out hom many times need to load the blocks for the input relation
	unsigned int size = ((in.size() + MAX_MEMORY_BLOCKS - 2)) / (MAX_MEMORY_BLOCKS - 1);
	
	//set the out block pointer to last block of the buffer 
	block_t<T> *bufferOut = buffer + MAX_MEMORY_BLOCKS - 1;
	
	//process the blocks loaded into the buffer each time
	for (unsigned int i = 0; i < size; i++)
	{
		int recordcounts = 0;
		int nblocks=readBlocks<T>(in, buffer, (MAX_MEMORY_BLOCKS - 1), i*(MAX_MEMORY_BLOCKS - 1));
		
		recordPtr start = newPtr(0, buffer->maxRecords);
		//calculate the total records of the loaded blocks in the buffer 
		unsigned int offset = (nblocks - 1)*buffer->maxRecords + (buffer + nblocks - 1)->nreserved;
		recordPtr end = newPtr(offset, buffer->maxRecords);

		// starting from the very first record, all valid records in valid blocks are hashed
		for (; start < end; incr(start, buffer->maxRecords))
		{
			if (!buffer[start.block].valid) {
				start.record = buffer->maxRecords - 1;
				continue;
			}
			T record = getRecord<T>(buffer, start);
			if (record.getCol(field) == val)
			{
				recordcounts++;
				bufferOut->entries.push_back(record);
				bufferOut->nreserved++;
				if (bufferOut->nreserved == bufferOut->maxRecords)
				{
					out.push_back(*bufferOut);
					emptyBlock<T>(bufferOut, bufferOut->maxRecords);
				}
			}
		}
		printf("%d records are selected... \n",recordcounts);
	}
	//if there has any records left in the output buffer, push into the out vector
	if (bufferOut->nreserved != 0) out.push_back(*bufferOut);
	printf("End of selection operation.... \n");

}

#endif
