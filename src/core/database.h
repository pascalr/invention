#ifndef _DATABASE_H
#define _DATABASE_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "schema_generated.h"
#include <vector>
#include <sstream>
#include "SQLiteCpp/sqlite3/sqlite3.h"

std::string sanitizeQuote(std::string val);

template<class T>
std::string sanitizeQuote(T val) {
  std::stringstream ss; ss << val;
  return sqlite3_mprintf("%q", ss.str().c_str());
}

std::string sanitize(std::string str);

class LogQuery {
  public:

    LogQuery(SQLite::Statement& infoQuery) : info_query(infoQuery) {
      out << "[";
    }

    template<typename T>
    void bind(int index, T val) {
      out << "[\"";
      out << info_query.getColumnName(index);
      out << "\", ";
      out << sanitizeQuote(val);
      out << "]";
    }

    std::string print() {
      out << "]";
      return out.str();
    }

  protected:
    std::stringstream out;
    SQLite::Statement& info_query;
};

class Database {
  public:

    Database(const char* dbName) : db(dbName, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {
    }

    template <typename T>
    void log(const char* name, T val) {
      std::cout << "\033[35m" << name << "\033[0m" << ": " << val << std::endl;
    }

    template <typename T>
    void log(const char* name, T val, LogQuery& query) {
      std::cout << "\033[35m" << name << "\033[0m" << ": " << val << " " << query.print() << std::endl;
    }

    // Disabled because it is unsafe
    //void execute(const char* cmd) {
    //  log("DB EXEC", cmd);
    //  db.exec(cmd);
    //}

    // Optional used for like ORDER BY
    // It is added at the end of the query
    template<class T> 
    std::vector<T> all(std::string optional = "") {
      
      std::stringstream queryStr; queryStr << "SELECT * FROM " << getTableName<T>();
      queryStr << " " << sanitize(optional);

      std::vector<T> result;

      log("DB LOAD", queryStr.str());
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

      std::stringstream queryStr;
      queryStr << "SELECT * FROM " << getTableName<T>() << " WHERE id = " << id << " " << sanitize(optional) << " LIMIT 1";
      SQLite::Statement query(db, queryStr.str());

      T item;
      if (query.executeStep()) {
        parseItem(query, item);
        item.id = query.getColumn(0);
      }

      LogQuery logQuery(query);
      bindQuery(logQuery, item);
      log("DB LOAD", queryStr.str(), logQuery);

      return item;
    }

    // Optional used for like ORDER BY
    // It is added at the end of the query
    template<class T, class P> 
    T findBy(std::string columnName, P value, std::string optional = "") {

      std::stringstream queryStr;
      queryStr << "SELECT * FROM " << getTableName<T>() << " WHERE " << sanitize(columnName) << " = " << sanitizeQuote(value) << " " << sanitize(optional) << " LIMIT 1";
      SQLite::Statement query(db, queryStr.str());

      T item;
      if (query.executeStep()) {
        parseItem(query, item);
        item.id = query.getColumn(0);
      }

      LogQuery logQuery(query);
      bindQuery(logQuery, item);
      log("DB LOAD", queryStr.str(), logQuery);

      return item;
    }
   
    template<class T> 
    void clear() {
      std::stringstream ss; ss << "DELETE FROM " << getTableName<T>();
      log("DB CLEAR", ss.str());
      db.exec(ss.str());
    }
    
    template<class T> 
    void deleteFrom(std::string optional) {
      std::stringstream ss; ss << "DELETE FROM " << getTableName<T>() << " " << sanitize(optional);
      log("DB DELETE", ss.str());
      db.exec(ss.str());
    }

    template<class T> 
    void remove(T& item) {
      std::stringstream ss; ss << "DELETE FROM " << getTableName<T>() << " WHERE id = " << item.id;
      log("DB DELETE", ss.str());
      db.exec(ss.str());

      item.id = -1;
    }

    template<class T> 
    void insert(T& item) {
      SQLite::Statement infoQuery(db, "SELECT * FROM " + getTableName<T>() + " WHERE 0");
      LogQuery logQuery(infoQuery);

      std::string insertQuery = "INSERT INTO " + getTableName<T>() + " VALUES(NULL, ";
      for (int i = 1; i < infoQuery.getColumnCount(); i++) {
        insertQuery += "?";
        if (i != infoQuery.getColumnCount() - 1) {
          insertQuery += ", ";
        }
      }
      insertQuery += ")";
      
      bindQuery(logQuery, item);
      log("DB INSERT", "INSERT INTO " + getTableName<T>(), logQuery);

      SQLite::Statement query(db, insertQuery);
      bindQuery(query, item);
      query.exec();
      item.id = getLastInsertedId<T>();
    }

    template<class T> 
    void update(T& item) {
      SQLite::Statement infoQuery(db, "SELECT * FROM " + getTableName<T>() + " WHERE 0");
      LogQuery logQuery(infoQuery);

      std::string updateQuery = "UPDATE " + getTableName<T>() + " SET ";
      for (int i = 1; i < infoQuery.getColumnCount(); i++) {
        updateQuery += infoQuery.getColumnName(i); updateQuery += " = ?";
        if (i != infoQuery.getColumnCount() - 1) {
          updateQuery += ", ";
        }
      }
      updateQuery += " WHERE id = ?";
      
      bindQuery(logQuery, item);
      log("DB UPDATE", "UPDATE " + getTableName<T>() + " SET", logQuery);

      SQLite::Statement query(db, updateQuery);
      bindQuery(query, item);
      query.bind(infoQuery.getColumnCount(), item.id);
      query.exec();
    }

    SQLite::Database db;
    
    template<class T> 
    long unsigned int getMaxLength(std::string columnName) {
      std::stringstream ss; ss << "SELECT MAX(LENGTH("+sanitize(columnName)+")) FROM " << getTableName<T>();
      log("DB MAX", ss.str());
      SQLite::Statement query(db, ss.str());
      query.executeStep();
      return (int)query.getColumn(0);
    }
    
  protected:


    template<class T> 
    long getLastInsertedId() {
      // Probably more efficient to get the last rowid inserted, than get the id of that rowid, but I dont care for now.
      std::stringstream ss; ss << "SELECT MAX(id) FROM " << getTableName<T>();
      SQLite::Statement query(db, ss.str());
      query.executeStep();
      return query.getColumn(0);
    }

};

#endif
