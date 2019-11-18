REATE OR REPLACE FUNCTION search(IN msg text, lim integer DEFAULT 60)
RETURNS TABLE (content text, distance float)
IMMUTABLE AS
$$
SELECT content, distance
FROM
    (SELECT content, cube(get_vector(12, content::cstring))<->cube(get_vector(12, msg::cstring)) distance
    FROM content_table
    ORDER BY cube(get_vector(12, content::cstring))<->cube(get_vector(12, msg::cstring))
    LIMIT lim) tb
    WHERE similarity(msg, content) >= 0.6; -- pg_trgm
$$ LANGUAGE SQL;
