/*Project info :
	Yifeng Chen
	ID:5140309458 */
/* the best or nothing!*/


#include "cachelab.h"

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <math.h>

#define HIT 1
#define  MISS_HIT 2
#define MISS_EVICTION 3

typedef struct {
	int valid;
	long long last_visited;
	unsigned long long tag;
} Block;

typedef struct {
	int associativity;
	long long  empty;
	Block *block_list;
} Set;

typedef struct{
	int set_num;
	int associativity;
	int block_size;
	int tag_size;

	Set *set_list;
	int hits;
	int misses;
	int evictions;
} Cache;

void cache_intial(Cache *cache){
	cache->hits=0;
	cache->misses=0;
	cache->evictions=0;
	cache->tag_size = 64 - cache->set_num - cache->block_size;
	/* allocate memory for cache*/
	cache->set_list = (Set*) malloc (sizeof(Set) * pow(2,cache->set_num));
	long long i=0;
	long long j=0;
	for(;i < pow(2,cache->set_num);i++)
	{
		cache->set_list[i].block_list = (Block* ) malloc (sizeof(Block) * cache->associativity);
		cache->set_list[i].associativity=cache->associativity;
		cache->set_list[i].empty=cache->associativity;

		for(j=0;j < cache->associativity;j++){
			cache->set_list[i].block_list[j].valid=0;
			cache->set_list[i].block_list[j].tag=0;
			cache->set_list[i].block_list[j].last_visited = 0;
		}
	}
}


void refresh_set(Set *set)
{/* refresh the last visit value of all blocks after the set has been accessed */
	long long i;
	for(i=0;i < set->associativity;i++){
		set->block_list[i].last_visited ++;
	}
}

int find_victim(Set *set)
{/* find the block with largest last visited value within the set*/
	long long i;
	long long victim_index = 0;
	long long victim_last_visited = set->block_list[0].last_visited;
	for(i=1;i <  set->associativity;i++){
		if(set->block_list[i].last_visited > victim_last_visited){
			victim_last_visited = set->block_list[i].last_visited;
			victim_index = i;
		}
	}
	return victim_index;
}

int find_empty(Set *set)
{/* find the block with largest last visited value within the set*/
	long long i;
	for(i=0;i <  set->associativity;i++){
		if(set->block_list[i].valid == 0 ){
			break;
		}
	}
	return i;
}

int access_set(Set *set,long long unsigned int access_tag)
{
	long long  i = 0;
	for(;i < set->associativity;i++){
		if( set->block_list[i].tag == access_tag && set->block_list[i].valid == 1 ){
			/* hit */
			set->block_list[i].last_visited = -1;
			refresh_set(set);
			return HIT;
		}
	}
	if(set->empty > 0){
		long long empty_index = find_empty(set);
		set->block_list[empty_index].valid=1;
		set->block_list[empty_index].tag=access_tag;
		set->block_list[empty_index].last_visited = -1;
		set->empty--;
		refresh_set(set);
		return MISS_HIT;
	}else{
		long long victim_index = find_victim(set);
		set->block_list[victim_index].tag=access_tag;
		set->block_list[victim_index].last_visited = -1;
		refresh_set(set);
		return MISS_EVICTION;
	}
}

int access_cache(Cache *cache,char cmd,long long unsigned int access_address)
{
	unsigned long long  access_set_index  = access_address << cache->tag_size;
	access_set_index = access_set_index >> (cache->block_size + cache->tag_size);

	unsigned long long access_tag = access_address >> (64-cache->tag_size);
	int result = access_set( &(cache->set_list[ access_set_index ]) ,access_tag);
	switch(result){
		case HIT:
			if(cmd == 'M')
				cache->hits = cache->hits + 2;
			else
				cache->hits++;
			break;
		case MISS_HIT:
			cache->misses++;
			if(cmd == 'M')
				cache->hits++;
			break;
		case MISS_EVICTION:
			cache->evictions++;
			cache->misses++;
			if(cmd == 'M')
				cache->hits++;
			break;
	}
	return result;
}


void help_info(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}


int main(int argc,char **argv)
{
	FILE *trace_file;
	char *trace_name;
	Cache cache;
if (index != -1)
                      B[index][index] = temp ;
	int verbose_flag = 0;
	char cmd_char;
	cmd_char = getopt(argc,argv, "s:E:b:t:vh") ;
	/* read the command from terminal*/
	while(cmd_char != -1){
		switch(cmd_char)
		{
			case 's':
				cache.set_num = atol(optarg) ;
				printf("parameter s for number of sets:%d\n",cache.set_num);
				break;
			case 'E':
				cache.associativity = atol(optarg) ;
				printf("parameter E for associativity:%d\n",cache.associativity);
				break;
			case 'b':
				cache.block_size= atol(optarg) ;
				printf("parameter b for block size:%d\n",cache.block_size);
				break;
			case 't':
				trace_name = optarg;
				printf("parameter t for trace file:%s\n",trace_name );
				break;
			case 'v':
				verbose_flag = 1; 
				break;
			case 'h':
				help_info(argv);
				exit(0);
			default:
				printf("wrong command!\n");
				exit(1);
		}
		cmd_char = getopt(argc,argv, "s:E:b:t:vh") ;
	}
	/*intialize the cache*/
	cache_intial(&cache);
	/* read the trace file */
	trace_file  = fopen(trace_name, "r");
	char trace_char;
	long long address;
	int size;
	//int line = 0;

	if(trace_file != NULL){
		int valid_file_input =
					fscanf(trace_file," %c %llx,%d",&trace_char,&address,&size);
		while( valid_file_input == 3 ){
			//printf("read line %d \n",line);
			//line++;
			int result = -1;

			switch(trace_char){
				case 'I':
					printf(" %c %llx,%d\n",trace_char,address,size );
					break;
				case 'M' : case 'S' : case 'L' :
					result = access_cache(&cache,trace_char,address);
					break;
				default:
					printf("error command\n");
					break;
			}

			/* if verbose flag then print detailed information*/
			if(verbose_flag){
				printf(" %c %llx,%d",trace_char,address,size );
				switch(result){
					case HIT:
						if (trace_char == 'M')
							printf(",hit  hit\n");
						else
							printf(", hit\n");
						break;
					case MISS_HIT:
						if (trace_char == 'M')
							printf(", miss hit\n");
						else
							printf(", miss\n");
						break;
					case MISS_EVICTION:
						if (trace_char == 'M')
							printf(", miss eviction hit\n");
						else
							printf(", miss eviction\n");
						break;
				}
			}
			/* readin the next command*/
			valid_file_input =
					fscanf(trace_file," %c %llx,%d",&trace_char,&address,&size);
		}
	}/* when trace_file is NULL*/
	else{
		printf("error reading trace file!\n");
		exit(0);
	}

   	 printSummary(cache.hits, cache.misses, cache.evictions);
    	return 0 ;
}