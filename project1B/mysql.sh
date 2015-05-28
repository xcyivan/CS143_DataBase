#!/bin/bash
#mysql.sh

mysql="mysql"

# sql="show tables from TEST"
# $mysql -e "$sql"

case $1 in 
	Movie) 
		sql="select * from TEST.Movie where id<10";;
	Actor) 
		sql="select * from TEST.Actor where id<10";;
	Director)
		sql="select * from TEST.Director where id<10";;
esac

$mysql -e "$sql"




