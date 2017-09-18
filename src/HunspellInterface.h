/*
This file is part of DSpellCheck Plug-in for Notepad++
Copyright (C)2013 Sergey Semushin <Predelnik@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

#include "AbstractSpellerInterface.h"

#include "iconv.h"
#include "CommonFunctions.h"
#include "MainDef.h"

class Hunspell;

using WordSet = std::unordered_set<std::string>;
using SortedWordSet = std::set<std::string>;
struct iconv_wrapper_t {
    template <typename... ArgTypes>
    iconv_wrapper_t (ArgTypes &&... args)
    {
        conv = iconv_open (std::forward<ArgTypes> (args)...);
    }
    iconv_t get () const { return conv; }
    iconv_wrapper_t () {}

    ~iconv_wrapper_t () {
         if (conv != reinterpret_cast<iconv_t>(-1))
           iconv_close (conv);
    }

private:
    iconv_t conv = reinterpret_cast<iconv_t> (-1);
};

struct DicInfo {
  std::unique_ptr<Hunspell> hunspell;
  iconv_wrapper_t Converter;
  iconv_wrapper_t BackConverter;
  iconv_wrapper_t ConverterANSI;
  iconv_wrapper_t BackConverterANSI;
  std::wstring LocalDicPath;
  WordSet LocalDic; // Stored in Dictionary encoding
};

struct AvailableLangInfo {
  std::wstring name;
  int type; // Type = 1 - System Dir Dictionary, 0 - Nomal Dictionary

  bool operator<(const AvailableLangInfo &rhs) const {
    return name < rhs.name;
  }
};

class HunspellInterface : public AbstractSpellerInterface {
public:
  HunspellInterface(HWND NppWindowArg);
  ~HunspellInterface();
  __override virtual std::vector<std::wstring> GetLanguageList();
  __override virtual void SetLanguage(const wchar_t* Lang);
  __override virtual void SetMultipleLanguages(
      std::vector<wchar_t *> *List);             // Languages are from LangList
  __override virtual bool CheckWord(char *Word); // Word in Utf-8 or ANSI
  __override virtual bool IsWorking();
  __override virtual std::vector<std::string> GetSuggestions(const char* Word);
  __override virtual void AddToDictionary(char *Word);
  __override virtual void IgnoreAll(char *Word);

  void SetDirectory(wchar_t *Dir);
  void SetAdditionalDirectory(wchar_t *Dir);
  void WriteUserDic(WordSet *Target, wchar_t *Path);
  void ReadUserDic(WordSet *Target, const wchar_t* Path);
  void SetUseOneDic(bool Value);
  void UpdateOnDicRemoval(wchar_t *Path, bool &NeedSingleLangReset,
                          bool &NeedMultiLangReset);
  bool GetLangOnlySystem(wchar_t *Lang);

private:
    DicInfo* CreateHunspell(const wchar_t* Name, int Type);
  bool SpellerCheckWord(const DicInfo& Dic, char *Word, EncodingType Encoding);
  void MessageBoxWordCannotBeAdded();

public:
private:
  bool IsHunspellWorking;
  bool UseOneDic;
  wchar_t *DicDir;
  wchar_t *SysDicDir;
  std::set<AvailableLangInfo> dicList;
  std::map<std::wstring, DicInfo> AllHunspells;
  char *GetConvertedWord(const char *Source, iconv_t Converter);
  DicInfo *SingularSpeller;
  DicInfo *LastSelectedSpeller;
  std::vector<DicInfo *> m_spellers;
  WordSet Memorized;
  WordSet Ignored;
  bool InitialReadingBeenDone;
  wchar_t *UserDicPath;        // For now only default one.
  wchar_t *SystemWrongDicPath; // Only for reading and then removing
  HWND NppWindow;
};
