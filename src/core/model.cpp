#include "model.h"

void bindQuery(SQLite::Statement& query, const DetectedHRCode& item) {
  query.bind(1, item.coord.h);
  query.bind(2, item.coord.v);
  query.bind(3, item.coord.t);
  query.bind(4, item.centerX);
  query.bind(5, item.centerY);
  query.bind(6, item.scale);
  query.bind(7, item.imgFilename);
  query.bind(8, item.created_at);
  query.bind(9, item.updated_at);
  query.bind(10, item.jar_id);
  query.bind(11, item.weight);
  query.bind(12, item.content_name);
  query.bind(13, item.content_id);
  query.bind(14, item.lid_coord.x);
  query.bind(15, item.lid_coord.y);
  query.bind(16, item.lid_coord.z);
}

void parseItem(SQLite::Statement& query, DetectedHRCode& code) {
  code.coord = PolarCoord(query.getColumn(1), query.getColumn(2), query.getColumn(3));
  code.centerX = query.getColumn(4);
  code.centerY = query.getColumn(5);
  code.scale = query.getColumn(6);
  code.imgFilename = (const char*)query.getColumn(7);
  code.created_at = query.getColumn(8);
  code.updated_at = query.getColumn(9);
  code.jar_id = (const char*)query.getColumn(10);
  code.weight = (const char*)query.getColumn(11);
  code.content_name = (const char*)query.getColumn(12);
  code.content_id = (const char*)query.getColumn(13);
  code.lid_coord = UserCoord(query.getColumn(14), query.getColumn(15), query.getColumn(16));
}
    
void bindQuery(SQLite::Statement& query, const Jar& item) {
  query.bind(1, item.jar_format_id);
  query.bind(2, item.ingredient_id);
  query.bind(3, item.created_at);
  query.bind(4, item.updated_at);
  query.bind(6, item.jar_id);
}

void parseItem(SQLite::Statement& query, Jar& jar) {
  jar.jar_format_id = query.getColumn(1);
  jar.ingredient_id = query.getColumn(2);
  jar.created_at = query.getColumn(3);
  jar.updated_at = query.getColumn(4);
  jar.jar_id = query.getColumn(6);
}

template <> string getTableName<DetectedHRCode>() { return "detected_codes"; }
template <> string getTableName<Jar>() { return "jars"; }
//string getTableName(DetectedHRCode* code) { return "detected_codes"; }
//string getTableName(Jar* i) { return "jars"; }

//bool shelfHeightCmp(const Shelf& arg0, const Shelf& arg1) {
//  return arg0.height < arg1.height;
//}
