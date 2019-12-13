# Build
## Prepare

### CHANGE top directory Makefile

1. change for $LEVELDB_INC $ROCKSDB_INC $TITAN_INC for yourself %PATH% **INCLUDE**, you can also delete it if you dont have it
2. change $CC for yourself compiler,  such as g++
3. change $LDFLAGS, you can delete something you dont have, such as -ltitan.

### CHANGE sub-directory Makefile db/Makefile

1. the same as top, change the $xxxx_INC for yourself %PATH% **INCLUDE**

### CHANGE some files in db

1. if you dont have TITAN or ROCKSDB, you can delete `db/rocksdb_db.cc && db/rocksdb_db.h`
2. change the db/db_factory.cc, just delete the DB you dont have
3. change test_leveldb.sh $root_dir

### BUILD
1. cp /the/path/of/your/libleveldb.a ./static
2. make

### RUN
for more thing, you can read test_leveldb.sh and see the `ycsb.cc` for more arguments
```
./test_leveldb.sh leveldb load  # YCSB load data use leveldb
./test_leveldb.sh leveldb run   # YCSB run workload use leveldb
```


