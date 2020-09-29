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

    template <typename T>
    void log(const char* name, T val) {
      std::cout << "\033[35m" << name << "\033[0m" << ": " << val << endl;
    }

    void execute(const char* cmd) {
      std::lock_guard<std::mutex> guard(dbMutex);
      log("DB EXEC", cmd);
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
     
      table.items.clear(); 

      log("DB LOAD", queryStr.str());
      while (query.executeStep()) {
        T item = table.parseItem(query);
        item.id = query.getColumn(0);
        table.items.push_back(item);
      }
    }


    // Optional used for like ORDER BY
    // It is added at the end of the query
    template<class T> 
    std::vector<T> all(std::string optional = "") {
    //void all(std::vector<T> result, std::string optional = "") {
      std::lock_guard<std::mutex> guard(dbMutex);
      
      //stringstream queryStr; queryStr << "SELECT * FROM " << getTableName((T*)NULL);
      stringstream queryStr; queryStr << "SELECT * FROM " << getTableName<T>();
      queryStr << " " << optional;

      std::vector<T> result;

      SQLite::Statement query(db, queryStr.str());
      log("DB LOAD", queryStr.str());
      while (query.executeStep()) {
        T item; parseItem(query, item);
        item.id = query.getColumn(0);
        result.push_back(item);
      }
      return result;
    }

    // Optional used for like ORDER BY
    // It is added at the end of the query
    template<class T> 
    T find(int id, std::string optional = "") {

      std::lock_guard<std::mutex> guard(dbMutex);
      
      stringstream queryStr; queryStr << "SELECT * FROM " << getTableName<T>() << " WHERE id = " << id << " LIMIT 1";
      //stringstream queryStr; queryStr << "SELECT * FROM " << getTableName((T*)NULL) << " WHERE id = " << id << " LIMIT 1";
      queryStr << " " << optional;

      SQLite::Statement query(db, queryStr.str());
      log("DB LOAD", queryStr.str());
      if (query.executeStep()) {
        T item; parseItem(query, item);
        item.id = query.getColumn(0);
        return item;
      }
      T item;
      return item;
    }

    // Optional used for like ORDER BY
    // It is added at the end of the query
    template<class T, class P> 
    T findBy(std::string columnName, P value, std::string optional = "") {

      std::lock_guard<std::mutex> guard(dbMutex);
      
      stringstream queryStr; queryStr << "SELECT * FROM " << getTableName<T>() << " WHERE " << columnName << " = " << value << " LIMIT 1";
      //stringstream queryStr; queryStr << "SELECT * FROM " << getTableName((T*)NULL) << " WHERE id = " << id << " LIMIT 1";
      queryStr << " " << optional;

      SQLite::Statement query(db, queryStr.str());
      log("DB LOAD", queryStr.str());
      if (query.executeStep()) {
        T item; parseItem(query, item);
        item.id = query.getColumn(0);
        return item;
      }
      T item;
      return item;
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
      log("DB CLEAR", ss.str());
      db.exec(ss.str());
    }
    
    template<class T> 
    void insert(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
  
      log("DB INSERT", table.insert_query);
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

      vector<T>::iterator toRemove = table.items.end();
      for (auto it = table.items.begin(); it != table.items.end(); it++) {
        if (it->id == item.id) {toRemove = it; break;}
      }
      if (toRemove != table.items.end()) {
        table.items.erase(toRemove);
      }
    }*/

    template<class T> 
    void deleteFrom(std::string optional) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << getTableName<T>() << " " << optional;
      log("DB DELETE", ss.str());
      db.exec(ss.str());
    }

    template<class T> 
    void removeItem(Table<T>& table, int id) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << table.getTableName() << " WHERE id = " << id;
      cout << ss.str() << endl;
      db.exec(ss.str());

      typename vector<T>::iterator toRemove = table.items.end();
      for (auto it = table.items.begin(); it != table.items.end(); it++) {
        if (it->id == id) {toRemove = it; break;}
      }
      if (toRemove != table.items.end()) {
        table.items.erase(toRemove);
      }
    }

    // I want to use the same format, so I am not using UPDATE query.
    // TODO: Add a table: __updates to handle this with table_name:string, to_remove_id:int, to_add_id:int, so no record is lost
    // in the odd chance that the system crashes between a add item and a removeItem.
    template<class T> 
    void update(Table<T>& table, T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
   
      log("DB UPDATE", table.update_query);
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
