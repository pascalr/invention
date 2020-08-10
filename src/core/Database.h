#ifndef _DATABASE_H
#define _DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "Model.h"
#include <vector>

using namespace std;

// HUUUUUUUUGEEEE FIXME: I do not validate the user input. It could insert sql...
// I should probably use ruby on rails which does all this already...

#include <mutex>

class Database {
  public:

    Database(const char* dbName) : db(dbName, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    }


    template<class T> 
    void load(Table<T>& table) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      string queryStr = "SELECT rowid,* FROM ";
      queryStr += table.getTableName();
      SQLite::Statement query(db, queryStr);
      
      while (query.executeStep()) {
        T item = table.parseItem(query);
        item.rowid = query.getColumn(0);
        table.items.push_back(item);
      }
    }
    
    template<class T> 
    void clear(Table<T>& table) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << table.getTableName();
      db.exec(ss.str());
    }
    
    template<class T> 
    void addItem(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "INSERT INTO " << table.getTableName() << " VALUES(";
      ss << table.getValues(item) << ")";
      SQLite::Statement insertQuery(db, ss.str());
      table.bindBlob(insertQuery, item);
      insertQuery.exec();
    
      SQLite::Statement query(db, "SELECT last_insert_rowid()");
      query.executeStep();
      item.rowid = query.getColumn(0);
    
      table.items.push_back(item);
    }

    SQLite::Database db;
    
    std::mutex dbMutex;
};

#endif
