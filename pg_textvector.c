#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "utils/array.h"


#define N 3
#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261


#define is_match(code, length) (length >= 2) ? 1: (((code == 0x0020) || ((code >= 0x0030) && (code <= 0x0039)) || ((code >= 0x0041) && (code <= 0x005A)) || ((code >= 0x0061) && (code <= 0x007A))) ? 1: 0)


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(get_vector);


Datum
get_vector(PG_FUNCTION_ARGS)
{
	int32 HASH_BUCKETS = PG_GETARG_INT32(0);
	float8 hash_buckets[HASH_BUCKETS];
        text* input = PG_GETARG_TEXT_P(1);
	char* ptr_start = &(input->vl_dat);
	int32 input_size = VARSIZE(input) - VARHDRSZ;
	uint8_t counter = 0, length, length_counter, array_counter;
	uint32_t symbol_counter = 0, current_size = 0, hash_array[N] = {0};
        Datum* elems;
        ArrayType* hash_buckets_pg;
	
	memset(hash_buckets, 0, sizeof(hash_buckets));
	
        while (input_size > current_size)
        {
		length = pg_mblen(ptr_start + current_size);
		
		if (is_match((int)(*(ptr_start + current_size)), length))
		{
			counter++;
			symbol_counter++;
			
			for (array_counter = 0; array_counter < counter; array_counter++)
			{
				for (length_counter = 0; length_counter < length; length_counter++)
				{
					/* hash_array[array_counter] ^= (int)(*(ptr_start + current_size + length_counter)); 
					hash_array[array_counter] *= FNV_PRIME; */
					
					hash_array[array_counter] += (int)(*(ptr_start + current_size + length_counter));
					
				}
			}
			
			if (symbol_counter > (N - 1))
			{
				hash_buckets[(hash_array[counter % N]) % HASH_BUCKETS]++;
				hash_array[counter % N] = 0;
			}
			
			counter = (counter == N) ? 0: counter;
		}
		
		current_size += length;
		
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

