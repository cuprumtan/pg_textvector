#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"



PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(search_trigrams);
PG_FUNCTION_INFO_V1(test_text_args);
PG_FUNCTION_INFO_V1(count_bytes);
PG_FUNCTION_INFO_V1(count_symbols);


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
