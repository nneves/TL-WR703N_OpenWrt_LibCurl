//---------------------------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <string>
//---------------------------------------------------------------------------------------------
#include "ContainerLib.h"
//---------------------------------------------------------------------------------------------
// Enable/Disable printf debug (avoid sending data to USB Serial Port when not required)
#if 1
  #define debug(a) printf a
#else
  #define debug(a) (void)0
#endif

//---------------------------------------------------------------------------------------------
// constructor TContainerList
//---------------------------------------------------------------------------------------------
//TContainerElement *nullcontainerelement = new TContainerElement(0, "");
//TContainerList *nullcontainerlist = new TContainerList();

TContainerElement::TContainerElement(int iuuid, const char *pstrdata)
{
  uuid = iuuid;
  strdata = std::string(pstrdata);

  debug(("* ContainerElem(\"%d\")=%s\r\n", uuid, strdata.c_str()));
}

TContainerElement::~TContainerElement()
{
  debug(("ContainerElem(\"%d\") clear\r\n", uuid));
  strdata.clear();
}

int TContainerElement::GetID()
{
  return uuid;
}

const char *TContainerElement::GetData()
{
  return strdata.c_str();
}

//---------------------------------------------------------------------------------------------
// constructor TContainerList
//---------------------------------------------------------------------------------------------
TContainerList::TContainerList()
{
  debug(("Calling TContainerList Contructor\r\n"));

  autouuid = 0;
}
//---------------------------------------------------------------------------------------------
// destructor
//---------------------------------------------------------------------------------------------
TContainerList::~TContainerList()
{
  debug(("Calling TContainerList Destructor\r\n"));

  for(std::vector<TContainerElement*>::iterator it = vDataContainerElement.end()-1; it >= vDataContainerElement.begin(); --it)
  {
    debug(("Deleting TContainerList vector element (\"%d\"): %s\r\n", (*it)->GetID(), (*it)->GetData()));
    delete *it;
    *it = NULL;
  }

  // clear vector
  vDataContainerElement.clear();
}
//---------------------------------------------------------------------------------------------
// private function
//---------------------------------------------------------------------------------------------

void TContainerList::AddElement(const char *pstrdata)
{
  autouuid++;
  AddElementID(autouuid, pstrdata);
}
//---------------------------------------------------------------------------------------------

void TContainerList::AddElementID(int iuuid, const char *pstrdata)
{

  // create new Container Element object
  TContainerElement *pContainerElement = new TContainerElement(iuuid, pstrdata);
  // add Container Element to vector
  vDataContainerElement.push_back(pContainerElement);

  DisplayVectorData();
}
//---------------------------------------------------------------------------------------------

std::string TContainerList::PopFirstElement()
{
  std::string result;

  if(vDataContainerElement.size() <= 0)
    return result;

  debug(("Erase Vector Element\n"));
  std::vector<TContainerElement*>::iterator it = vDataContainerElement.begin();
  result = ((*it)->GetData());
  debug(("Erase Vector Element\n"));
  delete *it;
  *it = NULL;  
  debug(("Erase Vector Indice\n"));
  vDataContainerElement.erase(vDataContainerElement.begin());
  debug(("Completed Vector Erase\n"));
  
  return result;
}
//---------------------------------------------------------------------------------------------

int TContainerList::Count()
{
  return vDataContainerElement.size();
}
//---------------------------------------------------------------------------------------------

void TContainerList::DisplayVectorData()
{
  // iterate vector to retrieve Container Element objects
  for(std::vector<TContainerElement*>::iterator it = vDataContainerElement.begin(); it != vDataContainerElement.end(); ++it)
  {
    debug(("-> ContainerElement(\"%d\")=%s\r\n",(*it)->GetID(), (*it)->GetData()));
  }
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// public functions
//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------