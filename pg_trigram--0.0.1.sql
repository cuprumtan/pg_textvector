CREATE FUNCTION search_trigrams()
RETURNS TEXT AS 'pg_trigram', 'search_trigrams'
LANGUAGE C STRICT;

CREATE FUNCTION test_text_args(text)
RETURNS TEXT AS 'pg_trigram', 'test_text_args'
LANGUAGE C STRICT;

CREATE FUNCTION count_bytes(text)
RETURNS INTEGER AS 'pg_trigram', 'count_bytes'
LANGUAGE C STRICT;

CREATE FUNCTION count_symbols(text)
RETURNS INTEGER AS 'pg_trigram', 'count_symbols'
LANGUAGE C STRICT;

CREATE FUNCTION trigrams(text)
RETURNS TEXT AS 'pg_trigram', 'trigrams'
LANGUAGE C STRICT;
