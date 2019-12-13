//
// Created by wujy on 1/23/19.
//
#include <iostream>

#include "titandb_db.h"
#include "lib/coding.h"

using namespace std;

namespace ycsbc {
    TitanDB::TitanDB(const char *dbfilename, utils::Properties &props) :noResult(0){
    
        //set option
        rocksdb::titandb::TitanOptions options;
        SetOptions(&options, props);
        

        rocksdb::Status s = rocksdb::titandb::TitanDB::Open(options,dbfilename,&db_);
        if(!s.ok()){
            cerr<<"Can't open titandb "<<dbfilename<<" "<<s.ToString()<<endl;
            exit(0);
        }
    }

    void TitanDB::SetOptions(rocksdb::titandb::TitanOptions *options, utils::Properties &props) {

        //// 默认的Rocksdb配置
        options->create_if_missing = true;
        options->compression = rocksdb::kNoCompression;
        options->enable_pipelined_write = true;
		options->write_buffer_size = 4 * 1024 * 1024;
		options->target_file_size_base = 4 * 1024 * 1024;


		// save with LevelDB
		//options->level0_file_num_compaction_trigger = 4;
		//options->level0_slowdown_writes_trigger = 8;     
		//options->level0_stop_writes_trigger = 12;

        ////

        int dboption = stoi(props["dboption"]);

        if ( dboption == 1) {  //RocksDB-L0-NVM: L0 have file path 
#ifdef ROCKSDB_L0_NVM 
            printf("set Rocksdb_L0_NVM options!\n");
            options->level0_file_num_compaction_trigger = 4;
            options->level0_slowdown_writes_trigger = 112;     
            options->level0_stop_writes_trigger = 128;
            options->level0_file_path = "/pmem/nvm";
#endif
        }
        else if ( dboption == 2 ) { //Matrixkv: 
#ifdef MATRIXKV 
            printf("set Matrixkv options!\n");
            rocksdb::NvmSetup* nvm_setup = new rocksdb::NvmSetup();
            nvm_setup->use_nvm_module = true;
            nvm_setup->pmem_path = "/pmem/nvm";
            options->nvm_setup.reset(nvm_setup);
            options->max_background_jobs = 3;
            options->max_bytes_for_level_base = 8ul * 1024 * 1024 * 1024;
#endif 
        }
        
    }


    int TitanDB::Read(const std::string &table, const std::string &key, const std::vector<std::string> *fields,
                      std::vector<KVPair> &result) {
        string value;
        rocksdb::Status s = db_->Get(rocksdb::ReadOptions(),key,&value);
        if(s.ok()) {
            //printf("value:%lu\n",value.size());
            DeSerializeValues(value, result);
            /* printf("get:key:%lu-%s\n",key.size(),key.data());
            for( auto kv : result) {
                printf("get field:key:%lu-%s value:%lu-%s\n",kv.first.size(),kv.first.data(),kv.second.size(),kv.second.data());
            } */
            return DB::kOK;
        }
        if(s.IsNotFound()){
            noResult++;
            //cerr<<"read not found:"<<noResult<<endl;
            return DB::kOK;
        }else{
            cerr<<"read error"<<endl;
            exit(0);
        }
    }


    int TitanDB::Scan(const std::string &table, const std::string &key, int len, const std::vector<std::string> *fields,
                      std::vector<std::vector<KVPair>> &result) {
         auto it=db_->NewIterator(rocksdb::ReadOptions());
        it->Seek(key);
        std::string val;
        std::string k;
        //printf("len:%d\n",len);
        for(int i=0;i < len && it->Valid(); i++){
            k = it->key().ToString();
            val = it->value().ToString();
            //printf("i:%d key:%lu value:%lu\n",i,k.size(),val.size());
            it->Next();
        } 
        delete it;
        return DB::kOK;
    }

    int TitanDB::Insert(const std::string &table, const std::string &key,
                        std::vector<KVPair> &values){
        rocksdb::Status s;
        string value;
        SerializeValues(values,value);
        /* printf("put:key:%lu-%s\n",key.size(),key.data());
        for( auto kv : values) {
            printf("put field:key:%lu-%s value:%lu-%s\n",kv.first.size(),kv.first.data(),kv.second.size(),kv.second.data());
        } */
        s = db_->Put(rocksdb::WriteOptions(), key, value);
        if(!s.ok()){
            cerr<<"insert error\n"<<endl;
            exit(0);
        }
       
        return DB::kOK;
    }

    int TitanDB::Update(const std::string &table, const std::string &key, std::vector<KVPair> &values) {
        return Insert(table,key,values);
    }

    int TitanDB::Delete(const std::string &table, const std::string &key) {
        rocksdb::Status s;
        s = db_->Delete(rocksdb::WriteOptions(),key);
        if(!s.ok()){
            cerr<<"Delete error\n"<<endl;
            exit(0);
        }
        return DB::kOK;
    }

    void TitanDB::PrintStats() {
        cout<<"read not found:"<<noResult<<endl;
        string stats;
        db_->GetProperty("rocksdb.stats",&stats);
        cout<<stats<<endl;
    }

    bool TitanDB::HaveBalancedDistribution() {
		return true;
        //return db_->HaveBalancedDistribution();
    }

    TitanDB::~TitanDB() {
        printf("wait delete db\n");
        delete db_;
        printf("delete\n");
    }

    void TitanDB::SerializeValues(std::vector<KVPair> &kvs, std::string &value) {
        value.clear();
        PutFixed64(&value, kvs.size());
        for(unsigned int i=0; i < kvs.size(); i++){
            PutFixed64(&value, kvs[i].first.size());
            value.append(kvs[i].first);
            PutFixed64(&value, kvs[i].second.size());
            value.append(kvs[i].second);
        }
    }

    void TitanDB::DeSerializeValues(std::string &value, std::vector<KVPair> &kvs){
        uint64_t offset = 0;
        uint64_t kv_num = 0;
        uint64_t key_size = 0;
        uint64_t value_size = 0;

        kv_num = DecodeFixed64(value.c_str());
        offset += 8;
        for( unsigned int i = 0; i < kv_num; i++){
            ycsbc::DB::KVPair pair;
            key_size = DecodeFixed64(value.c_str() + offset);
            offset += 8;

            pair.first.assign(value.c_str() + offset, key_size);
            offset += key_size;

            value_size = DecodeFixed64(value.c_str() + offset);
            offset += 8;

            pair.second.assign(value.c_str() + offset, value_size);
            offset += value_size;
            kvs.push_back(pair);
        }
    }
}
