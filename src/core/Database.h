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
    
      string queryStr = "SELECT * FROM ";
      queryStr += table.getTableName();
      SQLite::Statement query(db, queryStr);
      
      while (query.executeStep()) {
        T item = table.parseItem(query);
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
      ss << "NULL" << ", " << table.getValues(item) << ")";
      SQLite::Statement insertQuery(db, ss.str());
      table.bindBlob(insertQuery, item);
      insertQuery.exec();
      item.id = getLastInsertedId(table);
    
      table.items.push_back(item);
    }
    
    template<class T> 
    void removeItem(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << table.getTableName() << "WHERE id = " << item.id;
      db.exec(ss.str());
    }

    // I want to use the same format, so I am not using UPDATE query.
    // TODO: Add a table: __updates to handle this with table_name:string, to_remove_id:int, to_add_id:int, so no record is lost
    // in the odd chance that the system crashes between a add item and a removeItem.
    template<class T> 
    void saveItem(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);

      removeItem(table, item);
      addItem(table, item);
    }

    SQLite::Database db;
    
    std::mutex dbMutex;

  protected:

    template<class T> 
    long getLastInsertedId(Table<T>& table) {
      stringstream ss; ss << "SELECT MAX(id) FROM " << table.getTableName();
      SQLite::Statement query(db, ss.str());
      query.executeStep();
      return query.getColumn(0);
    }
};

#endif
