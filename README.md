CSCI 8340 DBMS II
Term Project Final Report 
Simulation of implementing a join algorithm in a distributed database environment
Huaqing Li
lhqphd@gmail.com

1.	Design

Simulation of query processing helps to understand the working mechanism of a database management system. Join operation is possible the most expensive cost in query processing.  In a centralized database system, the measurement of the cost is focused on the number of block transfers from disk and the number of seeks while extra factors must be considered in a distributed database environment to evaluate a join method such as data shipping cost, and data availability. 
 
Fig. 1 Flow chart of the processing
In this study, the work focused on the design and implementation of hash join algorithm. The handling of the memory blocks is simulated to demonstrate how database system manage the buffer. Fig.1 represent the program flow chart for query processing. The function for each module:
1.	Get Input: Capture command line parameters.
2.	Parse Query: Parse the query statement and Store the parameters for next operations. 
3.	Select? : Check if query needs to do selection. 
4.	Selection: Find the records matches the comparison condition, return a table for next join operation.
5.	Hash join: Semi hash join (sj) or normal hash join (nj).
6.	Projection: display records with the specified fields.

The detailed information on each step is described as below. 
1.1 Query input and parameter parsing
The program is designed to use a command line parameters as user input interface. The parameter includes a request site name followed by query statement. The format for the query statement: select field1 [field2,…] from table1 sj/nj table2 [where comparison condition]. All the parameters are separated by a space. The sj represent semi join and nj is normal join. If the query includes selection operation, the table2 are the default relation for the select operation.
For example, SimQP NYC select account_number custome_name from depositor sj account where account_number A13025. SimQP is the program name, NYC is the request site name, and the left is the query statement. As shown in Fig.2, the program has two global flags which specify the site name for request site and current operator execution. At the beginning of the program, the request site name taken from the command line was used to set both flags. The current_site is dynamically update during the processing. The query statement is parsed into two groups. One group is the keywords (select, from, where) which are pushed into the command stack, another is the parameters which are pushed into the parameter stack. 
 
Fig. 2 The storage for the results from parsing the input parameters
1.2 Selection algorithm
The command stack first pop out a command. If the command name equals to “where”, then two parameters are pop out from the parameters stack. The first parameter is the value for comparison. The second parameter is the field used to compare. To determine which table should be used for the selection operator, a procedure may be used to find the right table according to the site flags and the table availability. If there need to ship data from one site to another site, current site name must be updated accordingly. 
Once all the information is ready, then the select function was called. In this function, one buffer block is used as output buffer (fig.3), the left buffer blocks were used as input buffer, therefore, at most of 4 blocks (because the maximum buffer blocks are 5) in the table can be loaded into the buffer each time. All the records in the input buffer block were iterated to compare with the condition, if matched with the field’s value, then put into the output block. If the output block is full, then write back to the output table in disk. Once all the blocks in the table have been processed, write the output block into output table if there are left records in the block. Finally, a reference name of the output table was pushed back into the parameters stack for next operation.  
 
Fig. 3. Schematic representation of the blocks management in the selection
1.3 Hash join algorithm
If the command stack pop out a command equals to “from”, which means the operation is to join two tables. Then three parameters are pop out from the parameters stack and the join type can be determined from the second parameter. The first and the third parameter are the two relation names. Calling the procedure to locate the right tables for the hash join procedure.
The hash join algorithm includes two phases: partition and probe. Briefly, a hash function is used to partition records of both relations into sets that have the same hash value on the join attributes. For each pair of partitions, one relation is used as build input, another is used as probe input. A hash index on the build input is built in buffer using a different hash function. The records in the probe input are iterated to look up the hash index to located all the records in the build input. If the join attribute value in the both relations is matched, then concatenate both records for normal join, or only output the record in the probe input for semi-join. 
 
Fig. 4. Schematic representation of the blocks management in the relation partitioning
The partition procedure involving the buffer blocks is shown in Fig. 4. The table has n blocks which are stored in disk. The last buffer block is used as input block and the others are output. Each time one block in the table is loaded into the buffer input. Each record from the input is taken out and the join attribute value is hashed using a hash function. The returned integer is used as the index of output block, and also the partition index. The record was then put into the corresponding output block. If the output block is full, then write the block back to the corresponding partition and empty the block for next fill. After all the blocks in the disk have been process, if there are still records left in some output blocks, then those blocks should write back to corresponding partition. In this study, assume five buffer blocks are available, then 4 of them can be used as output. To simplify the algorithm, all the relations are divided into 4 partitions.
In the probe phase, each pair of partition is processed each time. The buffer blocks are divided into 3 sections. The last block is used as output. The last but one block is used to load one block of probe input each time. The build input can be used the left buffer blocks. Because the size of each partition is assured to be less than 3 blocks in the partition step, therefore, 3 of the 5 buffer blocks are used to load all the blocks in the build input at one time. 
 
Fig. 5. schematic representation of the procedures for building in-memory hash index and probing the relations.
Fig.5. shows the process of building in-memory hash index and probing the relations. Assuming all the blocks in build input has been loaded into the blocks. To iterate the records in these blocks, a data structure is designed to represent a record pointer which stores the block id and the record index in the specific block. Each time the record indicated by the record pointer is taken out from the buffer block and a hash function is applied on the join attribute to generate an index. The record pointer are then inserted into a linked list array specified by the index.
In the probing step, each time one block in the probe input is loaded into the last but one buffer block. Each time one record in the block is taken out, and the same hash function as the one used in creating the hash index is employed to generate an index. This index tells which linked list should be used to look up. Following the block id and the record number in the linked list node to locate the record from build buffer blocks. The record is then retrieved and compared with the one from probe input. If both records are matched, then the fields value from probe input or both input depending on the join type are written to output.
