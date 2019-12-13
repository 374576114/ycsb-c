#/bin/bash

root_dir=/home/xp/YCSB-C-master
workload=${root_dir}/workloads/workloada.spec
dbpath=${root_dir}/nvme/ycsb

rmdb() {
	if [ -n "$dbpath" ];then
		echo 'We will delete all files in ' $dbpath
		rm -f $dbpath/*
	fi
}

if [ $# -lt 1 ]; then
	echo 'Usage : ./xxxx.sh [rock|level|titan] [load|run]'
	exit 0
fi

case $1 in
	'rock')
		case $2 in
			'load')
			    rmdb
				./ycsbc -db rocksdb -dbpath $dbpath -threads 1 -P $workload -load true -dboption 1
				;;
			'run')
				./ycsbc -db rocksdb -dbpath $dbpath -threads 1 -P $workload -run true -dboption 1
				;;
			*)
				echo 'Error INPUT'
				exit 0
				;;
		esac
		;;
	'leveldb')
		case $2 in
			'load')
			    rmdb
				./ycsbc -db leveldb -dbpath $dbpath -threads 1 -P $workload -load true -dboption 1
				;;
			'run')
				./ycsbc -db leveldb -dbpath $dbpath -threads 1 -P $workload -run true -dboption 1
				;;
			*)
				echo 'Error INPUT'
				exit 0
				;;
		esac
		;;
	'titan')
		case $2 in
			'load')
			    rmdb
				./ycsbc -db titandb -dbpath $dbpath -threads 1 -P $workload -load true -dboption 1
				;;
			'run')
				./ycsbc -db titandb -dbpath $dbpath -threads 1 -P $workload -run true -dboption 1
				;;
			*)
				echo 'Error INPUT'
				exit 0
				;;
		esac
		;;
	*)
		echo "Error INPUT"
		exit 0
		;;
esac
