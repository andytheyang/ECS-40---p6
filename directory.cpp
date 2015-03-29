// Author: Andy Yang
#include <cstring>
#include <iostream>
#include "file.h"
#include "directory.h"
#include "permissions.h"
#include "list.h"

using namespace std;

Directory::Directory(const char *nam, Directory *paren, const char *owner)
  : subDirectoryCount(0), parent(paren)
{
  name = new char[strlen(nam) + 1];
  strcpy(name, nam);
  permissions.set(0777, owner);
}  // Directory())


Directory::Directory(const Directory &rhs) :
  subDirectoryCount(rhs.subDirectoryCount),
  parent(rhs.parent)
{
  name = new char[strlen(rhs.name) + 1];
  strcpy(name, rhs.name);
  permissions = rhs.permissions;
  subDirectories = *(new List<File>());

  for (int i = 0; i < rhs.subDirectoryCount; i++)
  {
    const Directory *test = dynamic_cast<const Directory*>
                            (rhs.subDirectories[i]);

    if (test)
    {
      Directory *newDir = new Directory(*test);
      newDir->parent = this;
      subDirectories += newDir;
    }  // if Directory
    else  // not a Directory
    {
      File *newFile = new File(*rhs.subDirectories[i]);
      subDirectories += newFile;
    }  // else
  }  // for all subDirectories
}   // Directory copy constructor

Directory* Directory::cd(int argCount, const char *arguments[],
                         const char *user)
{
  if (argCount != 2)
  {
    cout << "usage: cd directoryName\n"; return this;
  }  // if two many arguments

  if (strcmp(arguments[1], "..") == 0)
  {
    if (parent)
      return parent;
    else  // if not
      return this;
  }  // if .. args

  for (int i = 0; i < subDirectoryCount; i++)
    if (*subDirectories[i] == Directory(arguments[1]))
    {
      Directory *target = dynamic_cast<Directory *> (subDirectories[i]);

      if (!target)
      {
        cout << arguments[1] << ": Not a directory." << endl; return this;
      }  // if target is a file

      if (subDirectories[i]->getPermissions()
          .isPermitted(EXECUTE_PERMISSIONS, user))
        return dynamic_cast<Directory*> (subDirectories[i]);	// nulldeath
      else  // subdirectory doesn't have execute permissions
      {
        cout << arguments[1] << ": Permission denied.\n"; return this;
      }   // else no execute permissions for the specified directory
    }  // if found the named directory

  cout << "cd: " << arguments[1] << ": No such file or directory\n";
  return this;
}  // cd()


bool Directory:: iscpCorrectFormat(int argCount, const char *arguments[])
{
  if (argCount != 3)
  {
    if (argCount == 1)
      cout << "cp: missing operand\n";
    else   // more than one argument, but not 3
      if (argCount == 2)
        cout << "cp: missing destination file operand after ‘" << arguments[1]
             << "’\n";
      else  // more than 3 arguments
        cout << "Too many operands\n";

    cout << "Try 'cp --help' for more information.\n";
    return false;
  }  // if incorrect number of operands

  if (strcmp(arguments[1], arguments[2]) == 0)
  {
    cout << "cp: ‘" << arguments[1] << "’ and ‘" << arguments[2]
      << "’ are the same file\n";
    return false;
  }  // if source and destination have the same name

  for (int i = 0; i < subDirectoryCount; i++)
    if (*subDirectories[i] == Directory(arguments[2]))
    {
      cout << "cp: '" << arguments[2] << "' already exists.\n";
      return false;
    }  // if the destination directory already exists

  return true;
}  //  iscpCorrectFormat()


void Directory::chmod(int argCount, const char *arguments[], const char *user)
 // changes the permissions of the arguments
{
  if (argCount < 3)
  {
    if (argCount == 1)
      cout << "chmod: missing operand\n";
    else   // argCount == 2
      cout << "chmod: missing operand after ‘" << arguments[1] << "’\n";

    cout << "Try 'chmod --help' for more information.\n";
    return;
  }   // if wrong number of arguments

  short newPermissions = Permissions::checkOctals(arguments[1]);
  int j;

  if (newPermissions < 0)
  {
    cout << "chmod: invalid mode: ‘" << arguments[1] << "’\n";
    cout << "Try 'chmod --help' for more information.\n";
    return;
  }  // if invalid permissions

  for (int i = 2; i < argCount; i++)
  {
    for (j = 0; j < subDirectoryCount; j++)
      if (Directory(arguments[i]) == *subDirectories[j])
      {
        subDirectories[j]->getPermissions().chmod(newPermissions, user);
        break;
      }  // if matched name of directory with argument

    if (j == subDirectoryCount)
      cout << "chmod: cannot access ‘" << arguments[i] 
           << "’: No such file or directory\n";
  }  // for each filename
}  // chmod()


void Directory::chown(int argCount, const char *arguments[])
{
  int j;
  
  if (argCount != 3)
  {
    if (argCount == 1)
      cout << "chown: missing operand\n";
    else  // 2 or more than 3 arguments
      if (argCount == 2)
        cout << "chown: missing operand after ‘" << arguments[1] << "’\n";
      else  // more than 3 arguments
        cout << "chown: Too many arguments.\n";

    cout << "Try 'chown --help' for more information.\n";
    return;
  }  // if wrong number of arguments
  
  for (j = 0; j < subDirectoryCount; j++)
     if (Directory(arguments[2]) == *subDirectories[j])
      {
        subDirectories[j]->getPermissions().chown(arguments[1]);
        break;
      }  // if matched name of directory with argument
    
  if (j == subDirectoryCount)
    cout << "chown: cannot access ‘" << arguments[2] 
         << "’: No such file or directory\n";
}  // chown()


void Directory::cp(int argCount, const char *arguments[], const char *user)
{
  if (!iscpCorrectFormat(argCount, arguments))
    return;

  for (int i = 0; i < subDirectoryCount; i++)
    if (*subDirectories[i] == Directory(arguments[1]))
    {
      if (!subDirectories[i]->getPermissions()
          .isPermitted(READ_PERMISSIONS, user))
      {
        cout << "cp: cannot open ‘" << arguments[1]
             << "’ for reading: Permission denied\n";
        return;
      }  // if not allowed to read

      Directory *test = dynamic_cast<Directory*> (subDirectories[i]);

      if (test)
      {
        Directory *newDir = new Directory(*test);
        newDir->name = new char[strlen(arguments[2]) + 1];
        strcpy(newDir->name, arguments[2]);
        subDirectories += newDir;
      }  // if test
      else  // if File
        subDirectories += new File(arguments[2],
                          new Permissions(subDirectories[i]->getPermissions()));
      subDirectoryCount++;
      return;
    }  // if found source subdirectory

  cout << "cp: cannot stat ‘" << arguments[1] 
       << "’: No such file or directory\n";
  cout << "Try 'cp --help' for more information.\n";
}  // cp())

void Directory::ls(int argCount, const char *arguments[],
                   const char *user) const
{
  if (argCount > 2 || (argCount == 2 && strcmp(arguments[1], "-l") != 0))
    cout << "usage: ls [-l]\n";
  else  // correct number of arguments
  {
    if (permissions.isPermitted(READ_PERMISSIONS, user))
    {
      for (int i = 0; i < subDirectoryCount; i++)
        subDirectories[i]->ls(argCount == 2);

      if (argCount == 1)
        cout << endl;
    }  // if have read permissions
    else // don't have read permissions
      cout << "ls: cannot open directory .: Permission denied\n";
  }  // else correct arguments
}  // ls()


void Directory::mkdir(int argCount, const char *arguments[], const char *user)
{
  int i;
  
  if (argCount == 1)
  {
    cout << "mkdir: missing operand\n";
    cout << "Try 'mkdir --help' for more information.\n";
    return;
  }  // if too many arguments
   
  for (int argPos = 1; argPos < argCount; argPos++)
  {
    for (i = 0; i < subDirectoryCount; i++)
      if (*subDirectories[i] == Directory(arguments[argPos]))
      {
        cout << "mkdir: cannot create directory ‘" << arguments[argPos] 
             << "’: File exists\n"; 
        break;
      }  // if subdirectory already exists.
    
    if (i == subDirectoryCount)
    {
      if (permissions.isPermitted(WRITE_PERMISSIONS, user))
      {
        subDirectories += new Directory(arguments[argPos], this, user);
        File::updateTime();
        subDirectoryCount++;
      }  // if have write permissions
      else // no write permissions
        cout << "mkdir: cannot create directory ‘" << arguments[argPos] 
             <<  "’: Permission denied\n";
    }  // if name not found
  }  // for each filename
}  // mkdir()

bool Directory::istouchCorrectFormat(int argCount, const char *arguments[])
{
  if (argCount == 1)
  {
    cout << "touch: missing operand" << endl;
    cout << "Try 'touch --help' for more information." << endl;
    return false;
  }  // if no specified targets
  else  // otherwise
    return true;
}  // istouchCorrectFormat()

void Directory::touch(int argCount, const char *arguments[], const char *user)
{
  if (!istouchCorrectFormat(argCount, arguments))
    return;

  for (int i = 1; i < argCount; i++)
  {
    File *target = NULL;

    for (int j = 0; j < subDirectoryCount; j++)
      if (*subDirectories[j] == Directory(arguments[i]))
      {
        target = subDirectories[j]; break;
      }  // if match

    if (permissions.isPermitted(WRITE_PERMISSIONS, user))
    {
      if (target)
      {
        if (target->getPermissions().isPermitted(WRITE_PERMISSIONS, user))
          target->touch();
        else  // if no permissions
          cout << "touch: Permissions denied." << endl;
      }  // if target exists
      else  // if target was never found
      {
        subDirectories += new File(arguments[i], new Permissions(0666, user));
        subDirectoryCount++;
        File::updateTime();
      }  // else
    }  // if permitted
    else  // if not
      cout << "touch: cannot create file '" << arguments[i]
           << "': Permission denied" << endl;
  }  // for all arguments
}  // touch()

void Directory::showPath() const
{
  if (parent == NULL)
  {
    cout << "/";
    return;
  }  // at root
  
  parent->showPath();
  cout << name << "/";
}  // showPath())


void Directory::read(istream &is)
{
  File::read(is);
  is >> subDirectoryCount;
  is.ignore(10, '\n');


  for (int i = 0; i < subDirectoryCount; i++)
  {
    char *type = new char[3];
    is >> type;

    if (type[0] == 'F')
    {
      File *newFile = new File("DumbFile");
      is >> *newFile;
//      newFile->read(is);
      subDirectories += newFile;
    }  // if File
    else  // if "DF"
    {
      Directory *newDir = new Directory("Dummy", this);
      is >> *newDir;
      subDirectories += newDir;
    }  // else
  }  // for all subdirectories
}  // read()


void Directory::write(ostream &os) const
{
  os << 'D';					// to declare dir
  File::write(os);
  os << ' ' << subDirectoryCount << endl;	// for subdirs

  for (int i = 0; i < subDirectoryCount; i++)
  {
    const Directory *test = dynamic_cast<const Directory*> (subDirectories[i]);

    if (test)
      os << *test;
    else  // if File
      os << *subDirectories[i];
  }  // for all subdirectories
}  // write

