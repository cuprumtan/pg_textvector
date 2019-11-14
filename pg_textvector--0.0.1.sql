CREATE FUNCTION get_vector(text)
RETURNS float8[] AS 'pg_textvector', 'get_vector'
LANGUAGE C STRICT;
