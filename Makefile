MODULES = pg_trigram
EXTENSION = pg_trigram
DATA = pg_trigram--0.0.1.sql
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
