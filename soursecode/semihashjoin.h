#pragma once
/*
* DBMS Implementation
*
* Contact: geopiskas@gmail.com
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <string.h>
#include <vector>

#include "dbmsproj.h"
#include "recordOps.h"
#include "bufferOps.h"
#define GET_VARIABLE_NAME(Variable) (#Variable)

/*
* seed: seed to use in hash function
* buffer: buffer used, already loaded with a relation to hash
* size: the size in blocks of the relation loaded on buffer
* field: which field will be used for joining
*
* returns the pointer to the hash index
*/
template <typename T>linkedRecordPtr** createHashIndex(char *seed, block_t<T> *buffer, unsigned int size, std::string const& field, int debugmode = 0) {
	//initialize
		
	// the hash index consists of a maximum of hashSize linked lists where
	// each list has pointers to the records with common hash value

	// the size of hashIndex
	unsigned int hashSize = size * buffer->maxRecords;
	linkedRecordPtr **hashIndex = (linkedRecordPtr**)malloc(hashSize * sizeof(linkedRecordPtr*));
	for (unsigned int i = 0; i < hashSize; i++) {
		hashIndex[i] = nullptr;
	}

	recordPtr start = newPtr(0,buffer->maxRecords);
	unsigned int offset = (size - 1)*buffer->maxRecords + (buffer+size-1)->nreserved;
	recordPtr end = newPtr(offset,buffer->maxRecords);

	// starting from the very first record, all valid records in valid blocks
	// are hashed
	for (; start < end; incr(start,buffer->maxRecords)) {
		if (!buffer[start.block].valid) {
			start.record = buffer->maxRecords - 1;
			continue;
		}
		T record = getRecord<T>(buffer, start);
		if (record.valid) {
			unsigned int index = hashRecord<T>(seed, record,hashSize,field);
			linkedRecordPtr *ptr = (linkedRecordPtr*)malloc(sizeof(linkedRecordPtr));
			ptr->ptr = start;
			ptr->next = hashIndex[index];
			hashIndex[index] = ptr;
		}
	}
	
	// returns hashIndex
	printf("Hash index is created successfully!\n");
	return hashIndex;
};

/*
* infile: filename of the file whose records will be joined with the ones on buffer
* inBlocks: size of infile
* buffer: the buffer that is used (a file is already loaded on it)
* nmem_blocks: size of buffer
* size: the size of the file already loaded on buffer
* out: file descriptor of the outfile
* nres: number of pairs
* nios: number of ios
* field: which field will be used for joining
*/

template <typename T1, typename T2> void hashAndProbe(std::vector<block_t<T1>> &r, block_t<T1> *buffer, block_t<T2>* buildbuffer,unsigned int size, std::string const& field, std::vector<block_t<T1>> &out, int debugmode = 0) {
	
	unsigned int mod;

	mod = size*buildbuffer->maxRecords;

	char *seed = "agagagepiggeeq331516166fwhfsfrs";

	// hash index for the records already on buffer is created
	linkedRecordPtr **hashIndex = createHashIndex<T2>(seed, buildbuffer,size, field);
	// pointer to the buffer block where blocks of infile are loaded
	block_t<T1> *bufferIn = buffer;
	// pointer to the last buffer block, where pairs for output are written
	block_t<T1> *bufferOut = buffer + 1;

	
	for (unsigned int i = 0; i < r.size(); i++) {
		// if the block loaded is invalid, loads the next one
		readBlocks<T1>(r, bufferIn, 1,i);
		if (!(*bufferIn).valid) {
			continue;
		}
		// each record of the loaded block is hashed
		// then the linked list of the hash index for the corresponding hash value
		// is examined, and if a record has same value as the current one, both
		// are written to the output block

		for (unsigned int j = 0; j < bufferIn->nreserved; j++) {
			T1 record = (*bufferIn).entries[j];
			if (record.valid) {
				unsigned int index = hashRecord<T1>(seed, record, mod,field);
				linkedRecordPtr *element = hashIndex[index];
				while (element) {
					T2 tmp = getRecord(buildbuffer, element->ptr);
					if (compareRecords<T1,T2>(record, tmp, field) == 0) {

						bufferOut->entries.push_back(record);
						bufferOut->nreserved++;
						//(*bufferOut).entries[(*bufferOut).nreserved++] = tmp;
						//(*nres) += 1;

						// if output block becomes full, writes it to the outfile
						// and empties it
						
						if (bufferOut->nreserved == bufferOut->maxRecords) {
							// writeBlocks(out, bufferOut, 1);
							out.push_back(*bufferOut);
							emptyBlock<T1>(bufferOut, bufferOut->maxRecords);
							bufferOut->blockid += 1;
						}
					}
					element = element->next;
				}
			}
		}
	}
	
	if (bufferOut->nreserved != 0) {
		out.push_back(*bufferOut);
	}
	destroyHashIndex(hashIndex, size);
};
template <typename T1, typename T2> void hashAndProbefull(std::vector<block_t<T1>> &r, block_t<T1> *buffer, block_t<T2>* buildbuffer, unsigned int size, std::string const& field, std::vector<join_t<T1,T2>> &joinout, int debugmode = 0) {

	printf("Building in-memory hash index and probing...\n");
	unsigned int mod;
	mod = size*buildbuffer->maxRecords;
	char *seed = "agagagepiggeeq331516166fwhfsfrs";
	
	// hash index for the records already on buffer is created
	linkedRecordPtr **hashIndex = createHashIndex<T2>(seed, buildbuffer, size, field);
	// pointer to the buffer block where blocks of infile are loaded
	block_t<T1> *bufferIn = buffer;
	// pointer to the last buffer block, where pairs for output are written
	block_t<T1> *bufferOut = buffer + 1;


	for (unsigned int i = 0; i < r.size(); i++) {
		// if the block loaded is invalid, loads the next one
		readBlocks<T1>(r, bufferIn, 1, i);
		printf("Loading 1 block of proble relation into input buffer block.\n");
		if (!(*bufferIn).valid) {
			continue;
		}
		// each record of the loaded block is hashed
		// then the linked list of the hash index for the corresponding hash value
		// is examined, and if a record has same value as the current one, both
		// are written to the output block

		for (unsigned int j = 0; j < bufferIn->nreserved; j++) {
			T1 record = (*bufferIn).entries[j];
			if (record.valid) {
	
				unsigned int index = hashRecord<T1>(seed, record, mod, field);
				printf("Get 1 record from input buffer, hash it \n");
				printf("Look up the hash index to match the records with same index=%d \n",index);
				linkedRecordPtr *element = hashIndex[index];
				while (element) {
					
					T2 tmp = getRecord(buildbuffer, element->ptr);
					if (compareRecords<T1, T2>(record, tmp, field) == 0) {

						bufferOut->entries.push_back(record);
						bufferOut->nreserved++;
						//(*bufferOut).entries[(*bufferOut).nreserved++] = tmp;
						//(*nres) += 1;

						// if output block becomes full, writes it to the outfile
						// and empties it
						printf("Join the two records. Get next record... \n");
						join_t<T1, T2> rec;
						rec.rec1 = record;
						rec.rec2 = tmp;
						joinout.push_back(rec);
						if (bufferOut->nreserved == bufferOut->maxRecords) {
							// writeBlocks(out, bufferOut, 1);
							printf("block is full, write back to disk. \n");
							emptyBlock<T1>(bufferOut, bufferOut->maxRecords);
							bufferOut->blockid += 1;
						}
					}
					element = element->next;
				}
			}
		}
	}
	
	destroyHashIndex(hashIndex, size);
	printf("End of probing. \n");
	printf("======================================= \n");
	printf("\n");
};
/*
* filename: the name of the file to be partitioned
* size: the size of the file
* seed: a seed for the hash function
* buffer: the buffer that is used
* nmem_blocks: size of buffer
* bucketFilenames: array with the filenames of the bucket files to be produced
* mod: to be used for hashing
* nios: number of ios
* field: which field will be used for joining
*/
template<typename T> void createBuckets(std::vector<block_t<T>> &r, block_t<T> *buffer, std::vector<std::vector<block_t<T>>> &partition, unsigned int mod, std::string const& field, int debugmod = 0) {
	
	
	unsigned int block_counts = r.size();
	block_t<T> *bufferIn = buffer + MAX_MEMORY_BLOCKS - 1;
		
	for (unsigned int i = 0; i <block_counts; i++)
	{
		
		*bufferIn = r[i];//load one block from r into the last buffer block.
		printf("Loading 1 block from disk into input buffer block. \n");
		unsigned int max_records = bufferIn->entries.size();
		for (unsigned int j = 0; j < max_records; j++)
		{
			T record = bufferIn->entries[j];
			if (record.valid)
			{
				unsigned int index = hashRecord<T>("1235peqwtpqtuqewptuqptup1qtptu3421-58-12-35", record, mod,field);
				buffer[index].nreserved++;
				buffer[index].entries.push_back(record);
				//printf("record is hashed and put into %dth buffer block. \n",index);
				if (buffer[index].nreserved == buffer->maxRecords)
				{
					printf("%dth buffer block is full, write back to %dth partition. \n",index,index);
					buffer[index].valid = true;
					partition[index].push_back(buffer[index]);
					printf("Empty %dth buffer block. \n",index);
					emptyBlock<T>(buffer + index, buffer->maxRecords);
				}
			}
		}
		printf("All the records in the input buffer have been processed, loading next block... \n");
	}
	printf("All the blocks in the disk have been processed.\n");
//put all non full blocks into the corresponding partition
	for (unsigned int i = 0; i < mod; i++)
	{
		if (buffer[i].nreserved != 0)
		{
			buffer[i].valid = true;
			partition[i].push_back(buffer[i]);
			emptyBlock<T>(buffer + i, buffer[i].entries.size());
		
		}
		
	}
	
	
	if (debugmod !=0)
	{
	printf("write all non-full buffer blocks back to corrsponding partition. \n");
	printf("The table has been partitioned into %d partitions: \n", mod);
	printf("=======================================================================\n");
	//display the results
	unsigned int numPartition = 0;
	unsigned int numBlocks=0;
	unsigned int numRecords = 0;

	for each (auto blocks in partition)
	{
		
		numBlocks = 0;
		for each (auto block in blocks)
		{
			printf("The %dth Partition, %dth block:\n",numPartition,numBlocks);
			block.printrecord();
			numBlocks += 1;
		}
		numPartition += 1;
		printf("  \n");
	}

	}
	printf("End of partitioning the table. \n");
	printf("==================================================== \n");
	printf("\n");
};


template <typename T1,typename T2> void SemiHashJoin(std::vector<block_t<T1>> &r, std::vector<block_t<T2>> &s, char* field, std::vector<block_t<T1>> &out,int debugmode=0) {

	//figure out how many partitions to create 
	unsigned int smallSize;
	if (r.size() > s.size()) smallSize = s.size(); else smallSize = r.size();
	unsigned int mod=4;
	/*mod = smallSize / (MAX_MEMORY_BLOCKS - 1);
    if (smallSize % (MAX_MEMORY_BLOCKS - 1)) mod += 1;
    if (mod > MAX_MEMORY_BLOCKS) mod = MAX_MEMORY_BLOCKS;
	*/
	//create the partitions for r and s;
	std::vector<std::vector<block_t<T1>>> r_partition(mod);
	block_t<T1> T1buffer[MAX_MEMORY_BLOCKS];
	std::vector<std::vector<block_t<T2>>> s_partition(mod);
	block_t<T2> T2buffer[MAX_MEMORY_BLOCKS];
	printf("Partitioning...\n");
	
	createBuckets<T1>(r, T1buffer,r_partition,mod,field);
	printf("The %s table has been partitioned into %d partitions: \n", GET_VARIABLE_NAME(s), mod);
	printf("=======================================================================\n");
	createBuckets<T2>(s, T2buffer, s_partition, mod, field);
	
	//process each partition each time
	for (unsigned int i = 0; i < mod; i++)
	{
		unsigned int inblocks=r_partition[i].size();
		block_t<T2> buildbuffer[MAX_MEMORY_BLOCKS-2];
		block_t<T1> buffer[2];
	    if (s_partition[i].size()!=0)
		{ 
		//load one of s partitions into build buffer 
			for (unsigned int j = 0; j < s_partition[i].size(); j++)
			{
				buildbuffer[j] = s_partition[i][j];
			}
			hashAndProbe(r_partition[i], buffer, buildbuffer, s_partition[i].size(), field, out);
		
		}
	}
}

template <typename T1, typename T2> void HashJoin(std::vector<block_t<T1>> &r, std::vector<block_t<T2>> &s, char* field, std::vector<join_t<T1,T2>> &out, int debugmode = 0) {

	//figure out how many partitions to create 
	unsigned int smallSize;
	if (r.size() > s.size()) smallSize = s.size(); else smallSize = r.size();
	unsigned int mod = 4;
	/*mod = smallSize / (MAX_MEMORY_BLOCKS - 1);
	if (smallSize % (MAX_MEMORY_BLOCKS - 1)) mod += 1;
	if (mod > MAX_MEMORY_BLOCKS) mod = MAX_MEMORY_BLOCKS;
	*/
	//create the partitions for r and s;
	std::vector<std::vector<block_t<T1>>> r_partition(mod);
	block_t<T1> T1buffer[MAX_MEMORY_BLOCKS];
	std::vector<std::vector<block_t<T2>>> s_partition(mod);
	block_t<T2> T2buffer[MAX_MEMORY_BLOCKS];
	char* name = GET_VARIABLE_NAME(r);


	printf("=======================================================================\n");	
	createBuckets<T1>(r, T1buffer, r_partition, mod, field);

	printf("The %s table has been partitioned into %d partitions: \n", GET_VARIABLE_NAME(s), mod);
	printf("=======================================================================\n");
	createBuckets<T2>(s, T2buffer, s_partition, mod, field);
		
	for (unsigned int i = 0; i < mod; i++)
	{
		unsigned int inblocks = r_partition[i].size();
		block_t<T2> buildbuffer[MAX_MEMORY_BLOCKS - 2];
		block_t<T1> buffer[2];

		if (s_partition[i].size()!=0)
		{ 
		//load one of s partitions into build buffer 
		
		for (unsigned int j = 0; j < s_partition[i].size(); j++)
		{
			buildbuffer[j] = s_partition[i][j];
			printf("Loading all blocks in %dth build partition into buffer. \n",j);
		}
		hashAndProbefull(r_partition[i], buffer, buildbuffer, s_partition[i].size(), field, out);
	}
  }
	
}