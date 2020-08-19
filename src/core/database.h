#ifndef _DATABASE_H
#define _DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "model.h"
#include <vector>

using namespace std;

// HUUUUUUUUGEEEE FIXME: I do not validate the user input. It could insert sql...
// I should probably use ruby on rails which does all this already...

// FIXME: Instead of db.insert(table, ...), it should be table.insert(...), the table should have a reference to the database when initiated.

#include <mutex>

class Database {
  public:

    Database(const char* dbName) : db(dbName, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    }

    void execute(const char* cmd) {
      std::lock_guard<std::mutex> guard(dbMutex);
      cout << "DB EXEC: " << cmd << endl;
      db.exec(cmd);
    }

    // Optional used for like ORDER BY
    // It is added at the end of the query
    template<class T> 
    void load(Table<T>& table, string optional = "") {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream queryStr; queryStr << "SELECT * FROM " << table.getTableName();
      queryStr << " " << optional;

      SQLite::Statement query(db, queryStr.str());
      table.column_count = query.getColumnCount();
      table.update_query = "UPDATE " + table.getTableName() + " SET ";
      table.insert_query = "INSERT INTO " + table.getTableName() + " VALUES(NULL, ";
      for (int i = 1; i < query.getColumnCount(); i++) {
        table.update_query += query.getColumnName(i); table.update_query += " = ?";
        table.insert_query += "?";
        if (i != query.getColumnCount() - 1) {
          table.update_query += ", ";
          table.insert_query += ", ";
        }
      }
      table.update_query += " WHERE id = ?";
      table.insert_query += ")";
      
      cout << "DB LOAD: " << queryStr.str() << endl;
      while (query.executeStep()) {
        T item = table.parseItem(query);
        item.id = query.getColumn(0);
        table.items.push_back(item);
      }
    }
   
    // Get should only query database if not inside table. 
    /*template<class T> 
    T get(Table<T>& table, long id) {
      std::lock_guard<std::mutex> guard(dbMutex);
      stringstream ss; ss << "SELECT * FROM " << table.getTableName() << " WHERE ID = " << id;
      SQLite::Statement query(db, ss.str());

    }*/
    
    template<class T> 
    void clear(Table<T>& table) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << table.getTableName();
      cout << "DB CLEAR: " << ss.str() << endl;
      db.exec(ss.str());
    }
    
    template<class T> 
    void insert(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
  
      cout << "DB INSERT: " << table.insert_query << endl;
      SQLite::Statement insertQuery(db, table.insert_query);
      table.bindQuery(insertQuery, item);
      insertQuery.exec();
      item.id = getLastInsertedId(table);
    
      table.items.push_back(item);
    }
   
    // FIXME: table.items erase not working segfault 
    /*template<class T> 
    void removeItem(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << table.getTableName() << " WHERE id = " << item.id;
      cout << ss.str() << endl;
      db.exec(ss.str());

      for (auto it = table.items.begin(); it != table.items.end(); it++) {
        if (it->id == item.id) {table.items.erase(it);}
      }
    }*/

    // I want to use the same format, so I am not using UPDATE query.
    // TODO: Add a table: __updates to handle this with table_name:string, to_remove_id:int, to_add_id:int, so no record is lost
    // in the odd chance that the system crashes between a add item and a removeItem.
    template<class T> 
    void update(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
   
      cout << "DB UPDATE: " << table.update_query << endl;
      SQLite::Statement updateQuery(db, table.update_query);
      table.bindQuery(updateQuery, item);
      updateQuery.bind(table.column_count, item.id);

      updateQuery.exec();
    }

    SQLite::Database db;
    
    std::mutex dbMutex;

  protected:

    template<class T> 
    long getLastInsertedId(Table<T>& table) {
      // Probably more efficient to get the last rowid inserted, than get the id of that rowid, but I dont care for now.
      stringstream ss; ss << "SELECT MAX(id) FROM " << table.getTableName();
      SQLite::Statement query(db, ss.str());
      query.executeStep();
      return query.getColumn(0);
    }
};

#endif
