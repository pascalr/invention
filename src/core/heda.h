#ifndef _HEDA_H
#define _HEDA_H

class Heda;

#include "position.h"
#include "../lib/serial.h"
#include "reader/reader.h"
#include "writer/writer.h"
#include "writer/log_writer.h"

#include "database.h"

class StoppedException : public exception {};

class MissingConfigException : public exception {};

// TODO: Rename x and y to h and v at some point.
#define AXIS_H 'h'
#define AXIS_V 'v'
#define AXIS_T 't'
#define AXIS_R 'r'

class Axis {
  public:
    Axis(char id) : id(id) {}
    char id;
    bool is_referenced = false;
};

class Heda {
  public:

    Heda(Writer& writer, Reader& reader, Database &db, Writer& serverWriter, Reader& serverReader) : axisH(AXIS_H), axisV(AXIS_V), axisT(AXIS_T), axisR(AXIS_R),
              m_reader(reader),
              m_writer(writer),
              server_writer(serverWriter),
              server_reader(serverReader),
              stack_writer("\033[38;5;215mStack\033[0m"),
              db(db) {
    
      loadConfig();
    }

    void loadConfig();

    Axis axisH;
    Axis axisV;
    Axis axisT;
    Axis axisR;

    void captureFrame(Mat& frame);

    Axis* axisByName(char name);

    void stop();

    // Get the state of the arduino and set Heda with it. (I dont change arduino much now,
    // mainly Heda. So this avoids doing a reference every time I change something to Heda.)
    void sync() {
      askPosition();
    }

    void info() {
      m_writer << "?";
    }

    double unitH(double unitX, double unitT, double reference) {
      return unitX - config.user_coord_offset_x + (cosd(unitT) * reference);
    }
    
    double unitX(double unitH, double unitT, double reference) {
      return unitH + config.user_coord_offset_x - (cosd(unitT) * reference); // Not tested
    }
    
    double unitZ(double unitT, double reference) {
      return config.user_coord_offset_z-(sind(unitT) * reference);
    }
    
    double unitV(double unitY) {
      return config.user_coord_offset_y - unitY;
    }

    double unitY(double unitV) {
      return config.user_coord_offset_y - unitV;
    }

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    // The x axis and the z axis are reversed
    // Warning: There are two solutions sometimes.
    // The offset is the offset between the two zeroes.
    PolarCoord toPolarCoord(const UserCoord c, double reference);

    // reference: What part of the arm is wanted to get the position? The tool? Which tool? The camera? etc
    UserCoord toUserCoord(const PolarCoord p, double reference);

    UserCoord getCameraPosition() {
      return toUserCoord(m_position, config.camera_radius);
    }

    UserCoord getToolPosition() {
      return toUserCoord(m_position, config.gripper_radius);
    }
    
    PolarCoord getPosition() {
      return m_position;
    }

    Reader& m_reader;
    Writer& m_writer;

    Writer& server_writer;
    Reader& server_reader;

    LogWriter stack_writer;

    PolarCoord m_position;

    Database& db;

    HedaConfig config;

    Jar gripped_jar;
    bool is_gripping = false;

    void shelfByHeight(Shelf& shelf, double userHeight);

    vector<Shelf> shelves;
    vector<Shelf> storage_shelves;
    Shelf working_shelf;

    bool is_paused = false;

    // Checks if the server sent a "stop" message. Discards anything else.
    // Maybe it's OK for Heda to be only doing one command at a time.
    // The server can keep in memory what the stack of Heda commands.
    void ensureActive();

    string waiting_message;
    string fatal_message;
    string action_required;

    string user_response;
    
  protected:

    void askPosition();

    string m_pending_commands;

};

#endif
