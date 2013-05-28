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

  debug(("* ContainerElem[\"%d\"]=%s\r\n", uuid, strdata.c_str()));
}

TContainerElement::~TContainerElement()
{
  debug(("ContainerElem[\"%d\"] clear\r\n", uuid));
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
}
//---------------------------------------------------------------------------------------------
// destructor
//---------------------------------------------------------------------------------------------
TContainerList::~TContainerList()
{
  debug(("Calling TContainerList Destructor\r\n"));

  for(std::vector<TContainerElement*>::iterator it = vDataContainerElement.end()-1; it >= vDataContainerElement.begin(); --it)
  {
    debug(("Deleting TContainerList vector element [\"%d\"]: %s\r\n", (*it)->GetID(), (*it)->GetData()));
    delete *it;
    *it = NULL;
  }

  // clear vector
  vDataContainerElement.clear();
}
//---------------------------------------------------------------------------------------------
// private function
//---------------------------------------------------------------------------------------------
void TContainerList::AddElement(int iuuid, const char *pstrdata)
{

  // create new Container Element object
  TContainerElement *pContainerElement = new TContainerElement(iuuid, pstrdata);
  // add Container Element to vector
  vDataContainerElement.push_back(pContainerElement);

  DisplayVectorData();
}
//---------------------------------------------------------------------------------------------

void TContainerList::DisplayVectorData()
{
  // iterate vector to retrieve Container Element objects
  for(std::vector<TContainerElement*>::iterator it = vDataContainerElement.begin(); it != vDataContainerElement.end(); ++it)
  {
    debug(("-> ContainerElement[\"%d\"]=%s\r\n",(*it)->GetID(), (*it)->GetData()));
  }
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// public functions
//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------