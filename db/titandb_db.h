//
// 
//

#ifndef YCSB_C_TITANDB_DB_H
#define YCSB_C_TITANDB_DB_H

#include "core/db.h"
#include <iostream>
#include <string>
#include "core/properties.h"
#include <titan/db.h>
#include <titan/options.h>

using std::cout;
using std::endl;

namespace ycsbc {
    class TitanDB : public DB{
    public :
        TitanDB(const char *dbfilename, utils::Properties &props);
        int Read(const std::string &table, const std::string &key,
                 const std::vector<std::string> *fields,
                 std::vector<KVPair> &result);

        int Scan(const std::string &table, const std::string &key,
                 int len, const std::vector<std::string> *fields,
                 std::vector<std::vector<KVPair>> &result);

        int Insert(const std::string &table, const std::string &key,
                   std::vector<KVPair> &values);

        int Update(const std::string &table, const std::string &key,
                   std::vector<KVPair> &values);


        int Delete(const std::string &table, const std::string &key);

        void PrintStats();

        bool HaveBalancedDistribution();


        ~TitanDB();

    private:
        rocksdb::titandb::TitanDB *db_;
        unsigned noResult;

        void SetOptions(rocksdb::titandb::TitanOptions *options, utils::Properties &props);
        void SerializeValues(std::vector<KVPair> &kvs, std::string &value);
        void DeSerializeValues(std::string &value, std::vector<KVPair> &kvs);

    };
}


#endif //YCSB_C_TITANDB_DB_H
