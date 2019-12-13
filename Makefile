LEVELDB_INC=-I/home/xp/leveldb-cuda/include
ROCKSDB_INC=-I/home/xp/rocksdb-6.4.6/include -I/home/xp/rocksdb-6.4.6/
TITAN_INC=-I/home/xp/titan/include
LOCAL_INC=-I./

INCLUDE=$(LEVELDB_INC) $(ROCKSDB_INCLUDE) $(TITAN_INC) $(LOCAL_INC)

LIB=-L./static

CC=/usr/local/cuda-10.1/bin/nvcc
CFLAGS=-std=c++11 #-pthread
LDFLAGS= -lpthread -lrocksdb -lleveldb -ltitan
SUBDIRS= core db 
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc

all: $(SUBDIRS) $(EXEC)

$(SUBDIRS):
	$(MAKE) -C $@
	#$(MAKE) -C $@ ROCKSDB_INCLUDE=${ROCKSDB_INCLUDE}

$(EXEC): $(wildcard *.cc) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@ $(INCLUDE) $(LIB)
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)

.PHONY: $(SUBDIRS) $(EXEC)

