MODULES = pg_textvector
EXTENSION = pg_textvector
DATA = pg_textvector--0.0.1.sql
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
