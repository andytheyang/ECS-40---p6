#ifndef FILE_H
  #define FILE_H

#include "permissions.h"
#include "Time.h"
#include <iostream>

class File
{
private:
  Time time;
protected:
  char *name;
  Permissions permissions;
  void updateTime();
public:
  File(const char *nam, Permissions *perm = new Permissions(0777));
  File();
  File(const File &rhs);
  virtual ~File();
  Permissions& getPermissions();
  void touch();
  virtual void read(istream &is);
  virtual void write(ostream &os) const;
  void ls(bool isLongFormat) const;
  bool operator== (const File &rhs) const;
  bool operator< (const File &rhs) const;
  friend ostream& operator<< (ostream &os, File const &rhs);
  friend istream& operator>> (istream &is, File &rhs);
};  // class File

#endif
