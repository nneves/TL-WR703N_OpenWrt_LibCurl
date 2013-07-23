#ifndef __CONTAINERLIBH
#define __CONTAINERLIBH
//---------------------------------------------------------------------------------------------
// CONTAINER Library
//---------------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
// Date:      28-May-2013
// Objective: Container List using Vector
//
//---------------------------------------------------------------------------------------------
#include <vector>
#include <string>
#include <mutex>
//---------------------------------------------------------------------------------------------
class TContainerList;
class TContainerElement;
//---------------------------------------------------------------------------------------------

class TContainerElement
{
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  int uuid;
  std::string strdata;

  //-------------------------------------------------------------------------------------------
  public:
  //-------------------------------------------------------------------------------------------
  TContainerElement(int iuuid, const char *pstrdata);
  ~TContainerElement();

  int GetID();
  const char *GetData();
};
//---------------------------------------------------------------------------------------------

class TContainerList
{
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  std::vector<TContainerElement*>vDataContainerElement; // Vector container for the Container Element
  std::string buffer;
  int autouuid;

  std::mutex protectcontainer;
  std::mutex protectautouuid;

  //-------------------------------------------------------------------------------------------
  public:
  //-------------------------------------------------------------------------------------------
  TContainerList();
  ~TContainerList();

  void AddElement(const char *pstrdata);
  void AddElementID(int iuuid, const char *pstrdata);
  std::string PopFirstElement();
  int Count();
  void DisplayVectorData();  
};
//---------------------------------------------------------------------------------------------
#endif