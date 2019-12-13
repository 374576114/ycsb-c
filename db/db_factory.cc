//
//  basic_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/db_factory.h"

#include <string>
#include "db/basic_db.h"
#include "db/rocksdb_db.h"
#include "db/leveldb_db.h"
#include "db/titandb_db.h"

using namespace std;
using ycsbc::DB;
using ycsbc::DBFactory;

DB* DBFactory::CreateDB(utils::Properties &props) {
  if (props["dbname"] == "basic") {
    return new BasicDB;
  } else if (props["dbname"] == "rocksdb") {
    std::string dbpath = props.GetProperty("dbpath","/tmp/test-rocksdb");
    return new RocksDB(dbpath.c_str(), props);
  } else if (props["dbname"] == "leveldb") {
    std::string dbpath = props.GetProperty("dbpath","/tmp/test-leveldb");
    return new LevelDB(dbpath.c_str(), props);
  } else if (props["dbname"] == "titandb") {
    std::string dbpath = props.GetProperty("dbpath","/tmp/test-titandb");
	return new TitanDB(dbpath.c_str(), props);
  } else {
	  return NULL;
  }
}

