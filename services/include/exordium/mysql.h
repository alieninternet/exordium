/*   
 This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                           $
 $Author$
 $Date$
 $Id$
*/

#ifndef MYSQL_H
#define MYSQL_H

#include <mysql/mysql.h>

typedef MYSQL_FIELD MysqlField;
typedef MYSQL_ROW MysqlRow;

class Mysql;

struct c_base {
  Mysql *ref;
};

struct list_dbs_c : public c_base {
  const char *wild;
};

struct list_fields_c : public c_base {
  const char *table;
  const char *wild;
};

struct list_processes_c : public c_base {
};

struct list_tables_c  : public c_base {
  const char *wild;
};

struct store_result_c : public c_base {
};

struct use_result_c : public c_base {
};

class Mysql {
	friend class MysqlRes;
private:
	MYSQL *mysql;
  	int is_connected;
public:
  	int connected() {return is_connected;}
  	int connect (const char *host, const char *user, const char *passwd) {
    	if (mysql_connect(mysql, host, user, passwd))
      		return is_connected = 1;
    	else
      		return is_connected = 0;
  	}
  	void     close() {mysql_close(mysql);}

  	Mysql () {mysql = new MYSQL;}
  	Mysql (const char *host, const char *user, const char *passwd) {
    		mysql = new MYSQL;
    		connect (host, user, passwd);
  	}
  	~Mysql () {close(); delete mysql;}

  	int      affected_rows() {return mysql_affected_rows(mysql);}
  	int      create_db (const char *db) {return mysql_create_db(mysql, db);}
  	int      drop_db (const char *db) {return mysql_drop_db(mysql, db);}
  	char     *error () {return mysql_error(mysql);}
  	char     *get_clinet_info () {return mysql_get_client_info();}
  	char     *get_host_info () {return mysql_get_host_info(mysql);}
  	int      get_proto_info () {return mysql_get_proto_info(mysql);}
  	char     *get_server_info () {return mysql_get_server_info(mysql);}
  	char     *info () {return mysql_info(mysql);}
  	int      insert_id () {return mysql_insert_id(mysql);}
  	int      query (const char *query) {return mysql_query (mysql, query);}
  	int      real_query (const char *query, uint length)
      		       {return mysql_real_query (mysql, query, length);}
	int      reload() {return mysql_reload(mysql);}
  	int      select_db (const char *db) {return mysql_select_db(mysql,db);}
  	int      shutdown () {return mysql_shutdown(mysql);}
  	char     *stat() {return mysql_stat(mysql);}

  	list_dbs_c *list_dbs (const char *wild = "%") {
    		list_dbs_c *info = new list_dbs_c;
    		info->wild = wild;
    		info->ref = this;
    	return info; }

  	list_fields_c *list_fields (const char *table, const char *wild = "%") {
    		list_fields_c *info = new list_fields_c;
    		info->table = table;
    		info->wild = wild;
    		info->ref = this;
    	return info; }

  	list_processes_c *list_processes () {
    		list_processes_c *info = new list_processes_c;
    		info->ref = this;
    	return info; }

  	list_tables_c *list_tables (const char *wild = "%") {
    		list_tables_c *info = new list_tables_c;
    		info->wild = wild;
    		info->ref = this;
    	return info; }

  	store_result_c *store_result () {
    		store_result_c *info = new store_result_c;
    		info->ref = this;
    	return info; }
  
	use_result_c *use_result () {
    		use_result_c *info = new use_result_c;
    		info->ref = this;
    	return info; }
};

class MysqlRes
{
	private:
  		MYSQL_RES *mysql_res;
	public:
  		void         free_result() {mysql_free_result(mysql_res);}

  		MysqlRes () {mysql_res = new MYSQL_RES;}
  		~MysqlRes () {/*delete mysql_Res;*/}

  		int          eof () {return mysql_eof(mysql_res);}
  		void         data_seek (uint offset) {mysql_data_seek(mysql_res, offset);}
  		MysqlField   *fetch_field () {return mysql_fetch_field(mysql_res);}
  		unsigned int fetch_lengths () {return *mysql_fetch_lengths(mysql_res);}
  		MysqlRow     fetch_row() {return mysql_fetch_row(mysql_res);}
  		void         field_seek (int field) {mysql_field_seek (mysql_res, field);}
  		int          num_fields() {return mysql_num_fields(mysql_res);}
  		int          num_rows() {return mysql_num_rows(mysql_res);}

  		MysqlRes (list_dbs_c *info) 
    			{mysql_res = new MYSQL_RES; operator = (info);}
  		MysqlRes (list_fields_c *info) 
    			{mysql_res = new MYSQL_RES; operator = (info);}
  		MysqlRes (list_processes_c *info) 
    			{mysql_res = new MYSQL_RES; operator = (info);}
  		MysqlRes (list_tables_c *info)
    			{mysql_res = new MYSQL_RES; operator = (info);}
  		MysqlRes (store_result_c *info)
    			{mysql_res = new MYSQL_RES; operator = (info);}
  		MysqlRes (use_result_c *info)
    			{mysql_res = new MYSQL_RES; operator = (info);}

  		MysqlRes &operator = (list_dbs_c *info) {
    			mysql_res = mysql_list_dbs (info->ref->mysql, info->wild); 
    			delete info;
    			return *this;
  		}

  		MysqlRes &operator = (list_fields_c *info) {
    			mysql_res = mysql_list_fields (info->ref->mysql, info->table, info->wild);
    			delete info;
    			return *this;
  		}

  MysqlRes &operator = (list_processes_c *info) {
    mysql_res = mysql_list_processes (info->ref->mysql);
    delete info;
    return *this;
  }
  MysqlRes &operator = (list_tables_c *info) {
    mysql_res = mysql_list_tables (info->ref->mysql, info->wild);
    delete info;
    return *this;
  }
  MysqlRes &operator = (store_result_c *info) {
    mysql_res = mysql_store_result (info->ref->mysql); 
    delete info;
    return *this;
  }
  MysqlRes &operator = (use_result_c *info) {
    mysql_res = mysql_use_result (info->ref->mysql);
    delete info;
    return *this;
  }
};

#endif

