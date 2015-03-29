#include <iostream>
#include <cstring>
#include "file.h"
#include "permissions.h"

File::File(const char *nam, Permissions *perm)
{
  name = new char[strlen(nam) + 1];
  strcpy(name, nam);

  permissions = *perm;
}  // File()

File::File()
{
  // idk what to put in here
}  // File() default constructor

File::File(const File &rhs) : permissions(rhs.permissions)
{
  if (name)
    delete [] name;

  name = new char[strlen(rhs.name) + 1];
  strcpy(name, rhs.name);
}  // File() copy constructor

File::~File()
{
  delete [] name;
}  //~File()

Permissions& File::getPermissions()
{
  return permissions;
}  // getPermissions()

void File::touch()
{
  updateTime();	// sets time to current
}  // touch()

void File::read(istream &is)
{
  is >> name >> time >> permissions;
  is.ignore(10, '\n');
}  // read()

void File::write(ostream &os) const
{
  os << "F " << name << ' ' << time << ' ' << permissions
     << endl;
}  // write()

void File::ls(bool isLongFormat) const
{
  if (isLongFormat)
  {
    permissions.print();
    cout << ' ';
    time.print();
    cout << name << endl;
  }  // if longFormat
  else  // if regular ls
  {
    cout << name << " ";
  }  // else
}  // ls()

void File::updateTime()	// this is protected
{
  time.update();
}  // updateTime()

bool File::operator== (const File &rhs) const
{
  return strcmp(name, rhs.name) == 0;
}  // operator==()

bool File::operator< (const File &rhs) const
{
  return strcmp(name, rhs.name) < 0;
}  // operator<()

ostream& operator<< (ostream &os, File const &rhs)
{
  rhs.write(os);
  return os;
}  // operator<<()

istream& operator>> (istream &is, File &rhs)
{
  rhs.read(is);
  return is;
}  // operator>>()
