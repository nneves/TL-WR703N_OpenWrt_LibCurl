//---------------------------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <string>
//---------------------------------------------------------------------------------------------
#include "JsonParserLib.h"
//---------------------------------------------------------------------------------------------
// Enable/Disable printf debug (avoid sending data to USB Serial Port when not required)
#if 0
  #define debug(a) printf a
#else
  #define debug(a) (void)0
#endif

//---------------------------------------------------------------------------------------------
// constructor TJsonElement
//---------------------------------------------------------------------------------------------
TJsonElement *nulljsonelement = new TJsonElement("null", "");
TJsonParser *nulljsonparser = new TJsonParser("null");

TJsonElement::TJsonElement(const char *pProp, const char *pValue)
{
  property = pProp;
  value = pValue;
  fvaluejson = false;
  pvaluejson = NULL;

  // remove '"' from Begin
  if(property[0] == '\"')
    property.erase(0, 1);
  // remove '"' from End
  if(property[property.length()-1] == '\"')
    property.erase(property.length()-1, 1);

  // remove '"' from Begin
  if(value[0] == '\"')
    value.erase(0, 1);
  // remove '"' from End
  if(value[value.length()-1] == '\"')
    value.erase(value.length()-1, 1);

  // verify is value is a complex JSON object
  if(value.find("{")!=std::string::npos || value.find("}")!=std::string::npos)
  {
    fvaluejson = true;
    debug(("FOUND COMPLEX JSON VALUE, Parsing with internal TJsonParser object\r\n"));

    // creating an internal TJsonParser object to expand the COMPLEX JSON VALUE
    pvaluejson = new TJsonParser(value.c_str());
  }
  debug(("* JsonElem[\"%s\"]=%s\r\n", property.c_str(), value.c_str()));
}

TJsonElement::~TJsonElement()
{
  debug(("JsonElem[\"%s\"] clear\r\n", property.c_str()));
  property.clear();
  value.clear();
  if(pvaluejson)
  {
    debug(("Deleting internal TJsonParser object\r\n"));
    delete pvaluejson;
    pvaluejson = NULL;
  }
}

std::string TJsonElement::Prop()
{
  return property;
}

std::string TJsonElement::Value()
{
  return value;
}

bool TJsonElement::IsValueJSON()
{
  return fvaluejson;
}

TJsonParser *TJsonElement::ValueJson()
{
  if(!fvaluejson)
    return nulljsonparser;

  return pvaluejson;
}

TJsonElement* TJsonElement::Prop(const char *pPropName)
{
  return this->ValueJson()->Prop(pPropName);
}
//---------------------------------------------------------------------------------------------
// constructor TJsonParser
//---------------------------------------------------------------------------------------------
TJsonParser::TJsonParser(const char *pBuffer)
{
  debug(("Calling TJsonParser Contructor\r\n"));
  buffer = pBuffer;

  debug(("Setting internal Buffer=%s\r\n", buffer.c_str()));

  ParseString();
}
//---------------------------------------------------------------------------------------------
// destructor
//---------------------------------------------------------------------------------------------
TJsonParser::~TJsonParser()
{
  debug(("Calling TJsonParser Destructor\r\n"));

  for(std::vector<TJsonElement*>::iterator it = vDataJsonElement.end()-1; it >= vDataJsonElement.begin(); --it)
  {
    debug(("Deleting TJsonParset vector element: %s\r\n", (*it)->Prop().c_str()));
    delete *it;
    *it = NULL;
  }
}
//---------------------------------------------------------------------------------------------
// private function
//---------------------------------------------------------------------------------------------
void TJsonParser::ParseString()
{
  int ipropcounter = 0;
  debug(("Initialize JSON object parsing procedure ...\r\n"));

  // clear vector
  vDataJsonElement.clear();

  // remove { and } at the begin and end of original JSON string
  std::string::iterator itBegin = buffer.begin();
  if(*itBegin == '{')
  {
    debug(("Removing '{' char from begin of buffer\r\n"));
    buffer.erase(itBegin);
  }
  std::string::iterator itEnd = buffer.end();
  itEnd--;
  if(*itEnd == '}')
  {
    debug(("Removing '}' char from end of buffer\r\n"));
    buffer.erase(itEnd);
  }
  debug(("Buffer=%s\r\n", buffer.c_str()));

  // iterate buffer to parse by ',' that are not inside " "
  std::string::iterator itStart = buffer.begin();
  std::string::iterator itFind = buffer.begin();
  std::string::iterator itFinish = buffer.begin();
  bool fStart = false;
  bool fFind = false;
  bool fFinish = false;
  for(std::string::iterator it=buffer.begin(); it!=buffer.end(); ++it)
  {
    debug(("-> Data: %c\r\n", *it));

    // verify internal json element: "a":{"internal":"data"}
    if(*it == '{')
    {
      ipropcounter++;
      debug(("Internal Property Counter = %d\r\n", ipropcounter));
    }
    if(*it == '}')
    {
      ipropcounter--;
      if(ipropcounter < 0)
        ipropcounter = 0;
      debug(("Internal Property Counter = %d\r\n", ipropcounter));
    }

    if(!fStart && !fFind && !fFinish && ipropcounter == 0)
    {
      // Start
      itStart = it;
      fStart = true;
      debug(("Found Start: %c\r\n", *it));
    }
    if(fStart && !fFind && !fFinish && *it == ':' && ipropcounter == 0)
    {
      // Property separator ':'
      itFind = it;
      fFind = true;
      debug(("Found Separator: %c\r\n", *it));
    }
    if(fStart && fFind && !fFinish && ipropcounter == 0 && (*it == ',' || *it == '}' || (it+1)==buffer.end()))
    {
      // Finish
      itFinish = it;
      fFinish = true;
      debug(("Found Finish: %c\r\n", *it));
    }

    // found Start + Find + Finish
    if(fStart && fFind && fFinish && ipropcounter == 0)
    {
      debug(("Parsing JSON property:value ...\r\n"));

      fStart = false;
      fFind = false;
      fFinish = false;

      // extract JSON property
      std::string jsonprop;
      while(itStart != itFind)
      {
        jsonprop.push_back(*itStart);
        itStart++;
      }
      debug(("JSON Property: %s\r\n", jsonprop.c_str()));

      // extract JSON value
      std::string jsonvalue;
      itStart++;
      while(itStart != itFinish)
      {
        jsonvalue.push_back(*itStart);
        itStart++;
      }
      if((itStart+1) == buffer.end())
      {
        debug(("Special case, end of parsing\r\n"));
        jsonvalue.push_back(*itStart);
      }
      debug(("JSON Value: %s\r\n", jsonvalue.c_str()));

      itStart = it;
      itFind = it;
      itFinish = it;

      debug(("JSONELEMENT[%s]=%s\r\n", jsonprop.c_str(), jsonvalue.c_str()));

      // create new JSON Element object
      TJsonElement *pJsonElement = new TJsonElement(jsonprop.c_str(), jsonvalue.c_str());
      // add JsonElem to vector
      vDataJsonElement.push_back(pJsonElement);
    }
  }
  DisplayVectorData();
}
//---------------------------------------------------------------------------------------------

void TJsonParser::DisplayVectorData()
{
  // iterate vector to retrieve JSON Element objects
  for(std::vector<TJsonElement*>::iterator it = vDataJsonElement.begin(); it != vDataJsonElement.end(); ++it)
  {
    if((*it)->IsValueJSON())
    {
      debug(("-> JsonElement[\"%s\"]=%s [JSON_DATA]\r\n",(*it)->Prop().c_str(), (*it)->Value().c_str()));
      (*it)->ValueJson()->DisplayVectorData();
    }
    else
    {
      debug(("-> JsonElement[\"%s\"]=%s\r\n",(*it)->Prop().c_str(), (*it)->Value().c_str()));
    }
  }
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// public functions
//---------------------------------------------------------------------------------------------
TJsonElement *TJsonParser::Prop(const char *pPropName)
{
  TJsonElement *pResult = NULL;
  std::string auxdata = pPropName;

  for(std::vector<TJsonElement*>::iterator it = vDataJsonElement.begin(); it != vDataJsonElement.end(); ++it)
  {
    if((*it)->Prop() == auxdata)
    {
      pResult = *it;
      debug(("Found JsonElement[\"%s\"]=%s\r\n",(*it)->Prop().c_str(), (*it)->Value().c_str()));
      break;
    }
  }
  // if no record found returns temporary null
  if(!pResult)
    pResult = nulljsonelement;

  return pResult;
}
//---------------------------------------------------------------------------------------------