/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning
  IndexCursor cursor;
  BTreeIndex index;


  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;
  int    lookup;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  if ((index.open(table+".idx",'r')!=0){//if the index exists
    lookup = -1;

    //find a key to lookup
    for (unsigned i = 0; i < cond.size(); i++){
      // only consider conditions on keys
      if (cond[i].attr != 1)
        continue;

      // Only run this algorithm on the first condition on key
      if (cond[i].comp == SelCond::EQ){
        lookup = i;
        break;
      }
      // Determine a starting point of the range
      // and this algorithm only applies for GT or GE condition
      if (cond[i].comp == SelCond::GT || cond[i].comp == SelCond::GE){
        if (lookup == -1 || atoi(cond[i].value) > atoi(cond[lookup].value))
          lookup = i;
      }
    }

    
    if (lookup > -1)//locate the target entry in BTreeIndex
      index.locate(atoi(cond[lookup].value), cursor);
    else// no target, just start with the first entry
      index.locate(0,cursor);

    // tarversal the tree
    count = 0;
    while (!index.readForward(cursor, key, rid)){

      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto exit_select;
      }

      // Check the conditions
      for (unsigned i = 0; i < cond.size(); i++){
        // Compute the difference betweent the tuple and the condition
        switch (cond[i].attr){
          case 1:
            diff = key - atoi(cond[i].value);
            break;
          case 2:
            diff = strcmp(value.c_str(), cond[i].value);
            break;
        }

        // Determine if condition has been met
        switch (cond[i].comp){
        case SelCond::EQ:
          if (diff != 0)
            if (cond[i].attr == 1)
              goto finish_read;
            else
              continue;
          break;
        case SelCond::NE:
          if (diff == 0) continue;
          break;
        case SelCond::GT:
          if (diff <= 0) continue;
          break;
        case SelCond::LT:
          if (diff >= 0)
            if (cond[i].attr == 1)
              goto finish_read;
            else
              continue;
          break;
        case SelCond::GE:
          if (diff < 0) continue;
          break;
        case SelCond::LE:
          if (diff > 0)
            if (cond[i].attr == 1)
              goto finish_read;
            else
              continue;
          break;
        }
      }

      //if the switch breaks and we come here, this means this entry meets the condition so we add it!
      count++;

      // print the tuple 
      switch (attr) {
      case 1:  // SELECT key
        fprintf(stdout, "%d\n", key);
        break;
      case 2:  // SELECT value
        fprintf(stdout, "%s\n", value.c_str());
        break;
      case 3:  // SELECT *
        fprintf(stdout, "%d '%s'\n", key, value.c_str());
        break;
      }
    }
  }
  else{
    // scan the table file from the beginning
    rid.pid = rid.sid = 0;
    count = 0;
    while (rid < rf.endRid()) {
      // read the tuple
      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto exit_select;
      }

      // check the conditions on the tuple
      for (unsigned i = 0; i < cond.size(); i++) {
        // compute the difference between the tuple value and the condition value
        switch (cond[i].attr) {
        case 1:
  	diff = key - atoi(cond[i].value);
  	break;
        case 2:
  	diff = strcmp(value.c_str(), cond[i].value);
  	break;
        }

        // skip the tuple if any condition is not met
        switch (cond[i].comp) {
        case SelCond::EQ:
  	if (diff != 0) goto next_tuple;
  	break;
        case SelCond::NE:
  	if (diff == 0) goto next_tuple;
  	break;
        case SelCond::GT:
  	if (diff <= 0) goto next_tuple;
  	break;
        case SelCond::LT:
  	if (diff >= 0) goto next_tuple;
  	break;
        case SelCond::GE:
  	if (diff < 0) goto next_tuple;
  	break;
        case SelCond::LE:
  	if (diff > 0) goto next_tuple;
  	break;
        }
      }

      // the condition is met for the tuple. 
      // increase matching tuple counter
      count++;

      // print the tuple 
      switch (attr) {
      case 1:  // SELECT key
        fprintf(stdout, "%d\n", key);
        break;
      case 2:  // SELECT value
        fprintf(stdout, "%s\n", value.c_str());
        break;
      case 3:  // SELECT *
        fprintf(stdout, "%d '%s'\n", key, value.c_str());
        break;
      }

      // move to the next tuple
      next_tuple:
      ++rid;
    }
  }

  // print matching tuple count if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index){

  RC rc;
  RecordFile rf;
  ifstream is;

  if(rc=rf.open(table+".tbl",'w')){
    cout << "Cannot create table as RecordFile!"<<endl;
    return rc;
  }

  is.open(loadfile.c_str());
  if(!is.is_open()){
    cout<<"Cannot open source .del file!"<<endl;
    rc = -1;
    return rc;
  }

  BTreeIndex btindex;
  if (index){
    if(btindex.open(table+".idx",'w')){
      cout << "Error: Could Not Access Index" << endl;
      return 1;
    }
  }

  string line;
  while(getline(is,line)){
    int key;
    string value;
    RecordId rid;
    if(rc=parseLoadLine(line,key,value)){
      cout<<"Error when parsing lines from .del file! Please check if there is invalid text!"<<endl;
      return rc;
    }
    if(rc=rf.append(key,value,rid)){
      cout<<"Error when appending line at the end of record file!"<<endl;
      return rc;
    }

    if (index){
      if(btindex.insert(key, rid)){
        cout << "Warning: Could not insert key into index" << endl;
        continue;
      }
    }
  }
  cout<<"load successfully!"<<endl;
  rf.close();
  is.close();
  if (index)
    btindex.close();
  rc = 0;
  return rc;
  
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
