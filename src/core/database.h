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
    void log(T val, const char* name = "") {

      if (name[0] != '\0') {
        std::cout << "\033[35m" << name << "\033[0m" << ": ";
      }
      std::cout << val << endl;
    }

    void execute(const char* cmd) {
      std::lock_guard<std::mutex> guard(dbMutex);
      log(cmd, "DB EXEC");
      db.exec(cmd);
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

      log(queryStr.str(), "DB LOAD");
      SQLite::Statement query(db, queryStr.str());
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

      log(queryStr.str(), "DB LOAD");
      SQLite::Statement query(db, queryStr.str());
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
      
      stringstream queryStr; queryStr << "SELECT * FROM " << getTableName<T>() << " WHERE " << columnName << " = " << quoteValue(value) << " " << optional << " LIMIT 1";

      log(queryStr.str(), "DB LOAD");
      SQLite::Statement query(db, queryStr.str());
      if (query.executeStep()) {
        T item; parseItem(query, item);
        item.id = query.getColumn(0);
        return item;
      }
      T item;
      return item;
    }
   
    template<class T> 
    void clear() {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << getTableName<T>();
      log(ss.str(), "DB CLEAR");
      db.exec(ss.str());
    }
    
    template<class T> 
    void deleteFrom(std::string optional) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << getTableName<T>() << " " << optional;
      log(ss.str(), "DB DELETE");
      db.exec(ss.str());
    }

    template<class T> 
    void remove(T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
    
      stringstream ss; ss << "DELETE FROM " << getTableName<T>() << " WHERE id = " << item.id;
      log(ss.str(), "DB DELETE");
      db.exec(ss.str());

      item.id = -1;
    }

    template<class T> 
    void insert(T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
   
      SQLite::Statement infoQuery(db, "SELECT * FROM " + getTableName<T>() + " WHERE 0");

      std::string insertQuery = "INSERT INTO " + getTableName<T>() + " VALUES(NULL, ";
      for (int i = 1; i < infoQuery.getColumnCount(); i++) {
        insertQuery += "?";
        if (i != infoQuery.getColumnCount() - 1) {
          insertQuery += ", ";
        }
      }
      insertQuery += ")";
      
      log(insertQuery, "DB INSERT"); // TODO: I can log better than that, find a way to get values by indices

      SQLite::Statement query(db, insertQuery);
      bindQuery(query, item);
      query.exec();
      item.id = getLastInsertedId<T>();
    }

    template<class T> 
    void update(T& item) {
      std::lock_guard<std::mutex> guard(dbMutex);
   
      SQLite::Statement infoQuery(db, "SELECT * FROM " + getTableName<T>() + " WHERE 0");

      std::string updateQuery = "UPDATE " + getTableName<T>() + " SET ";
      for (int i = 1; i < infoQuery.getColumnCount(); i++) {
        updateQuery += infoQuery.getColumnName(i); updateQuery += " = ?";
        if (i != infoQuery.getColumnCount() - 1) {
          updateQuery += ", ";
        }
      }
      updateQuery += " WHERE id = ?";
      
      log(updateQuery, "DB UPDATE"); // TODO: I can log better than that, find a way to get values by indices

      SQLite::Statement query(db, updateQuery);
      bindQuery(query, item);
      query.bind(infoQuery.getColumnCount(), item.id);
      query.exec();
    }

    SQLite::Database db;
    
    std::mutex dbMutex;

  protected:

    template<class T>
    T quoteValue(T value) {
      return value;
    }
    
    std::string quoteValue(std::string value) {
      return "\"" + value + "\"";
    }


    template<class T> 
    long getLastInsertedId() {
      // Probably more efficient to get the last rowid inserted, than get the id of that rowid, but I dont care for now.
      stringstream ss; ss << "SELECT MAX(id) FROM " << getTableName<T>();
      SQLite::Statement query(db, ss.str());
      query.executeStep();
      return query.getColumn(0);
    }

};

#endif
