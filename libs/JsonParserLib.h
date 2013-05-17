#ifndef __JSONPARSERLIBH
#define __JSONPARSERLIBH
//---------------------------------------------------------------------------------------------
// JSON Parser Lib class (basic parser)
//---------------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
// Date:      15-Apr-2013
// Objective: micro-nano-pico JSON Parser C++ Lib with std::string, std::vector support (WIP)
//
// TODO: Array support not yet implemented
// TODO: Currently only returns strings values
//---------------------------------------------------------------------------------------------
#include <vector>
#include <string>
//---------------------------------------------------------------------------------------------
//typedef int int8_t; // remove this on MBED!!!
//---------------------------------------------------------------------------------------------
class TJsonParser;
class TJsonElement;
//---------------------------------------------------------------------------------------------

class TJsonElement
{
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  std::string property;
  std::string value;
  bool fvaluejson;
  TJsonParser *pvaluejson;

  //-------------------------------------------------------------------------------------------
  public:
  //-------------------------------------------------------------------------------------------
  TJsonElement(const char *pProp, const char *pValue);
  ~TJsonElement();

  std::string Prop();
  std::string Value();
  bool IsValueJSON();
  TJsonParser *ValueJson();
  TJsonElement* Prop(const char *pPropName);
};
//---------------------------------------------------------------------------------------------

class TJsonParser
{
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  std::vector<TJsonElement*>vDataJsonElement; // Vector container for the JSON Element
  std::string buffer;

  void ParseString();
  void DisplayVectorData();

  //-------------------------------------------------------------------------------------------
  public:
  //-------------------------------------------------------------------------------------------
  TJsonParser(const char *pBuffer);
  ~TJsonParser();

  TJsonElement* Prop(const char *pPropName);
};
//---------------------------------------------------------------------------------------------
#endif