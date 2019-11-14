#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "utils/array.h"


#define N 3
#define HASH_BUCKETS 12
#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261U


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(get_vector);


uint8_t
FNV1a(char* text, uint8_t len)
{

        uint32_t hash = FNV_OFFSET, i;

        for (i = 0; i < len; i++) {
                hash ^= (text[i]);
                hash *= FNV_PRIME;
        }

        return hash % HASH_BUCKETS;
}


Datum
get_vector(PG_FUNCTION_ARGS)
{
        // get input text and it's size in bytes
        text* input = PG_GETARG_TEXT_P(0);
        int32 input_size = VARSIZE(input) - VARHDRSZ;

        // create array to store vector values
        float8 hash_buckets[HASH_BUCKETS] = {0};

        char* Ngram;                        // to store Ngram
        char* ptr_start = &(input->vl_dat); // to store the start of Ngram
        //char* ptr_end = ptr_start;        // to shift to the end of Ngram
        uint8_t symbol_size, hash, Ngram_size, counter, offset;

        Datum* elems;
        ArrayType* hash_buckets_pg;

        while (input_size > 0)
        {
                Ngram_size = 0;

                // get Ngram size in bytes
                for (counter = 0; counter < N; counter++)
                {
                        symbol_size = pg_mblen(ptr_start + Ngram_size);
                        Ngram_size += symbol_size;
                        //ptr_end += symbol_size;
                }

                // allocate memory for Ngram and get it's hash
                //Ngram = (char*)malloc(Ngram_size * sizeof(char) + 1);
                //strncpy(Ngram, ptr_start, Ngram_size * sizeof(char));
                hash = FNV1a(ptr_start,Ngram_size);

                // destroy Ngram buffer
                //free(Ngram);

                // update hash bucket value
                hash_buckets[hash]++;

                // shift ptr_start to the start of the next Ngram
                offset = pg_mblen(ptr_start);
                ptr_start += offset;
                input_size -= offset;

        }

        elems = (Datum*)palloc(sizeof(hash_buckets) * sizeof(Datum));

        for (counter = 0; counter < HASH_BUCKETS; counter++)
        {
                elems[counter] = Float8GetDatum(hash_buckets[counter]);
        }

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


