#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "utils/array.h"


#define N 3
#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261U


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(get_vector);


uint8_t FNV1a(char* text, uint8_t len, int32 HASH_BUCKETS);


uint8_t
FNV1a(char* text, uint8_t len, int32 HASH_BUCKETS)
{
        uint32_t hash = FNV_OFFSET, i;
	
        for (i = 0; i < len; i++)
	{
                hash ^= (text[i]);
                hash *= FNV_PRIME;
        }
	
        return hash % HASH_BUCKETS;
}


Datum
get_vector(PG_FUNCTION_ARGS)
{
	int32 HASH_BUCKETS = PG_GETARG_INT32(0);
	float8 hash_buckets[HASH_BUCKETS];
        text* input = PG_GETARG_TEXT_P(1);
	char* ptr_start = &(input->vl_dat);
	int32 input_size = VARSIZE(input) - VARHDRSZ;
	uint8_t hash, counter, offset, Ngram_size = 0;
        Datum* elems;
        ArrayType* hash_buckets_pg;
	
	memset(hash_buckets, 0, sizeof(hash_buckets));
	
        while (input_size >= Ngram_size)
        {
                Ngram_size = 0;
		
                for (counter = 0; counter < N; counter++)
                        Ngram_size += pg_mblen(ptr_start + Ngram_size);
		
                hash = FNV1a(ptr_start, Ngram_size, HASH_BUCKETS);
		hash_buckets[hash]++;
		
                offset = pg_mblen(ptr_start);
                ptr_start += offset;
                input_size -= offset;
		
        }
	
        elems = (Datum*)palloc(sizeof(hash_buckets) * sizeof(Datum));
	
        for (counter = 0; counter < HASH_BUCKETS; counter++)
		elems[counter] = Float8GetDatum(hash_buckets[counter]);
	
        hash_buckets_pg = construct_array(
                                          elems,
                                          HASH_BUCKETS,
                                          701,
                                          sizeof(float8),
                                          FLOAT8PASSBYVAL,
                                          'd'
                                         );
	
        PG_RETURN_ARRAYTYPE_P(hash_buckets_pg);
}

