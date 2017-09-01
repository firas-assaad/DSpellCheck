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
#include <locale>
#include "MainDef.h"
#include "Plugin.h"

struct NppData;

void SetString(char *&Target, const char *Str);

void SetString(wchar_t *&Target, const wchar_t *Str);

void SetString(char *&Target, const wchar_t *Str);

void SetString(wchar_t *&Target, const char *Str);

// In case source is in UTF-8
void SetStringSUtf8(char *&Target, const char *Str);
void SetStringSUtf8(wchar_t *&Target, const char *Str);

// In case destination is in UTF-8
void SetStringDUtf8(char *&Target, const char *Str);
void SetStringDUtf8(char *&Target, const wchar_t *Str);

BOOL SetStringWithAliasApplied(wchar_t *&Target, const wchar_t *OrigName);

void SetParsedString(wchar_t *&Dest, wchar_t *Source);

LRESULT SendMsgToNpp(const NppData *NppDataArg, UINT Msg,
                     WPARAM wParam = 0, LPARAM lParam = 0);

char *Utf8Dec(const char *string, const char *current);
char *Utf8chr(const char *s, const char *sfc);
int Utf8Gewchar_tSize(char c);
char *Utf8strtok(char *s1, const char *Delimit, char **Context);
char *Utf8Inc(const char *string);
char *Utf8pbrk(const char *s, const char *set);
size_t Utf8Length(const char *String);
BOOL Utf8IsLead(char c);
BOOL Utf8IsCont(char c);

bool SortCompare(wchar_t *a, wchar_t *b);
bool Equivwchar_tStrings(wchar_t *a, wchar_t *b);
size_t Hashwchar_tString(wchar_t *a);
bool EquivCharStrings(char *a, char *b);
size_t HashCharString(char *a);
bool SortCompareChars(char *a, char *b);

BOOL CheckForDirectoryExistence(wchar_t *Path, BOOL Silent = TRUE,
                                HWND NppWindow = 0);
wchar_t *GetLastSlashPosition(wchar_t *Path);

// trim from start (in place)
inline void ltrim(std::wstring &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](wchar_t ch) {
        return !iswspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::wstring &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](wchar_t ch) {
        return !iswspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::wstring &s) {
    ltrim(s);
    rtrim(s);
}

struct TaskWrapper {
    explicit TaskWrapper(HWND targetHwnd);
    template <typename ActionType, typename GuiCallbackType>
     void doDeferred (ActionType action, GuiCallbackType guiCallback) {
    concurrency::create_task([=, as=m_aliveStatus]()
    {
        auto ret = action ();
        auto cbData = std::make_unique<CallbackData>();
        cbData->callback = [=](){ guiCallback (ret); };
        cbData->aliveStatus = as;
         PostMessage(m_targetHwnd,
            GetCustomGUIMessageId(CustomGUIMessage::GENERIC_CALLBACK), reinterpret_cast<WPARAM> (cbData.release ())
            , 0);
    });
}

private:
    HWND m_targetHwnd;
    std::shared_ptr<void> m_aliveStatus;
};

