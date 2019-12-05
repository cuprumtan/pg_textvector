CREATE FUNCTION get_vector_multihash(integer, text)
RETURNS float8[] AS 'pg_textvector', 'get_vector_multihash'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION get_vector_FNV1a(integer, text)
RETURNS float8[] AS 'pg_textvector', 'get_vector_FNV1a'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION get_vector_CRC8(integer, text)
RETURNS float8[] AS 'pg_textvector', 'get_vector_CRC8'
LANGUAGE C STRICT IMMUTABLE;
