CREATE FUNCTION get_vector(integer, cstring)
RETURNS float8[] AS 'pg_textvector', 'get_vector'
LANGUAGE C STRICT IMMUTABLE;
