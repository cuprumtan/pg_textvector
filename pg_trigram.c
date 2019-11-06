#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"



PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(search_trigrams);
PG_FUNCTION_INFO_V1(test_text_args);
PG_FUNCTION_INFO_V1(count_bytes);
PG_FUNCTION_INFO_V1(count_symbols);
PG_FUNCTION_INFO_V1(trigrams);


Datum
search_trigrams(PG_FUNCTION_ARGS)
{
	PG_RETURN_TEXT_P(cstring_to_text("Hello, World!"));
}

Datum
test_text_args(PG_FUNCTION_ARGS)
{
	text* sample_prefix = cstring_to_text("Prefix to input string --> ");
	int32 sample_prefix_size = VARSIZE(sample_prefix) - VARHDRSZ;
	
	text* sample_text = PG_GETARG_TEXT_P(0);
	int32 sample_text_size = VARSIZE(sample_text) - VARHDRSZ;

	text* sample_suffix = cstring_to_text(" <-- eol");
	int32 sample_suffix_size = VARSIZE(sample_suffix) - VARHDRSZ;
	
	int32 out_size = sample_prefix_size + sample_text_size + sample_suffix_size + VARHDRSZ;
	text* out = (text*)palloc(out_size);

	SET_VARSIZE(out, out_size);

	memcpy(VARDATA(out), VARDATA(sample_prefix), sample_prefix_size);
	memcpy(VARDATA(out) + sample_prefix_size, VARDATA(sample_text), sample_text_size);
	memcpy(VARDATA(out) + sample_prefix_size + sample_text_size, VARDATA(sample_suffix), sample_suffix_size);
	
	PG_RETURN_TEXT_P(out);
}

Datum
count_bytes(PG_FUNCTION_ARGS)
{
	text* input_text = PG_GETARG_TEXT_P(0);
	int32 input_text_size = VARSIZE(input_text) - VARHDRSZ;
	
	PG_RETURN_INT32(input_text_size);
}

Datum
count_symbols(PG_FUNCTION_ARGS)
{
	text* input_text = PG_GETARG_TEXT_P(0);
        int32 input_text_size = VARSIZE(input_text);
	text* out = (text*)palloc(input_text_size);
	SET_VARSIZE(out, input_text_size);
	memcpy(VARDATA(out), VARDATA(input_text), input_text_size);

	int32 symbols = 0, sz = input_text_size - VARHDRSZ, mlen = 0;
	char* ptr = VARDATA(out);
	while (sz > 0) {
		mlen = pg_mblen(ptr);
		ptr += mlen;
		symbols++;
		sz -= mlen;
	}
	
	PG_RETURN_INT32(symbols);
}


Datum
trigrams(PG_FUNCTION_ARGS)
{
	/* get input text and allocate memory */
	text* input_text = PG_GETARG_TEXT_P(0);
	int32 input_text_size = VARSIZE(input_text);	/* ignore VARHDRSZ because we have monolith text */
	text* out = (text*)palloc(input_text_size);
	SET_VARSIZE(out, input_text_size);
	memcpy(VARDATA(out), VARDATA(input_text), input_text_size);
		
	/* get number of characters */
	int32 count_symbols = 0;
	unsigned char mlen = 0;
	input_text_size -= VARHDRSZ;
	char* ptr = VARDATA(out);
	while (input_text_size > 0) {
                mlen = pg_mblen(ptr);
                ptr += mlen;
                count_symbols++;
                input_text_size -= mlen;
        }

	/* count trigrams number */
	int32 trigrams_count = count_symbols - 2;
	
	/* if trigrams <= 1 return input_text */
	if (trigrams_count <= 1) {
		PG_RETURN_TEXT_P(out);
	} else {
		/* create structure to store trigrams and allocate memory for struct array */
		typedef struct Trigram
		{
			text* value;
		};
		struct Trigram* trigram;
		trigram = malloc(trigrams_count*sizeof(struct Trigram*));

		/* create pointers to input_text */
		char* ptr_start = VARDATA(out);
		char* ptr_end = ptr_start;
		
		/* fill struct array with trigrams */
		unsigned char i, j;
		for (i = 0; i < trigrams_count; i++)
		{
			/* shift ptr_end to the end of trigram */
			for (j = 0; j < 3; j++)
			{
				mlen = pg_mblen(ptr_end);
				ptr_end += mlen;		
			}
			/* copy trigram into struct member */
			memcpy(VARDATA(trigram[i].value), ptr_start, ptr_end - ptr_start);
			/* shift ptr_start to the start of the next trigram */
			ptr_start = ptr_end;
		}
		/* return first trigram TEST! */
		PG_RETURN_TEXT_P(out);	
	}
}
