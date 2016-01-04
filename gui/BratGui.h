/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef __BratGui_H__
#define __BratGui_H__

#include "new-gui/brat/stdafx.h"

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)

#include "dnd_copy.xpm"
#include "dnd_move.xpm"
#include "dnd_none.xpm"

#include "InternalFiles.h"

using namespace brathl;

//#include "BratGui_wdr.h"
#include "wxGuiInterface.h"		// (to let wxWidgets choose a wxConfig class for your platform)
#include "new-gui/brat/Workspaces/Workspace.h"
#include "new-gui/brat/Workspaces/TreeWorkspace.h"
#include "GuiFrame.h"










#if defined(_WIN64) || defined(__LP64__) || defined(__x86_64__) 
	const wxString BRATGUI_TITLE_BASE = "Brat Interface (64 bit)";
#else
	const wxString BRATGUI_TITLE_BASE = "Brat Interface (32 bit)";
#endif
#if defined(DEBUG) || defined(_DEBUG)
	const wxString BRATGUI_TITLE = BRATGUI_TITLE_BASE + " [Debug]";
#else
	const wxString BRATGUI_TITLE = BRATGUI_TITLE_BASE;
#endif

const wxString BRAT_USER_MANUAL = wxString::Format("brat_user_manual_%s.pdf", BRAT_VERSION);
#ifdef __WXMAC__
const wxString BRAT_DOC_SUBDIR = "data";
#else
const wxString BRAT_DOC_SUBDIR = "doc";
#endif


const std::string DATASETS_PAGE_NAME = "Datasets";
const std::string OPERATIONS_PAGE_NAME = "Operations";
const std::string DISPLAY_PAGE_NAME = "Views";
const std::string LOG_PAGE_NAME = "Logs";


class CApplicationSettings;


// WDR: class declarations

//----------------------------------------------------------------------------
// BratApp
//----------------------------------------------------------------------------

class CBratGuiApp: public wxApp
{
public:
  CBratGuiApp();
  
  virtual bool OnInit();
  virtual int OnExit();

  static void DetermineCharSize(wxWindow* wnd, int32_t& width, int32_t& height);

  wxBitmapType GetIconType();
  wxString GetIconFile();
  wxString GetIconFileName();

private:
	wxFileConfig* GetConfig() {return m_config;}
	CApplicationSettings *mAppSettings = nullptr;

  bool SaveConfig(bool flush = true);
  bool SaveConfigSelectionCriteria(bool flush = true);
  
  bool LoadConfig();
  bool LoadConfigSelectionCriteria();

public:
  void LoadFileHistory( std::vector<std::string> &v );
  void SaveFileHistory( const std::vector<std::string> &v );

  void CreateTree(CWorkspace* root);
  void CreateTree(CWorkspace* root, CTreeWorkspace& tree);

  wxNotebook* GetMainnotebook()  { return  m_frame->GetMainnotebook(); }
  CGuiFrame* GetGuiFrame()  { return  m_frame; }
  CGuiPanel* GetGuiPanel()  { return  m_frame->GetGuiPanel(); }
  CLogPanel* GetLogPanel()  { return  m_frame->GetGuiPanel()->GetLogPanel(); }
  //CDisplayPanel* GetDisplayPanel()  { return  m_frame->GetGuiPanel()->GetDisplayPanel(); }
  CDisplayPanel* GetDisplayPanel()  { return  m_frame->GetGuiPanel()->GetDisplayPanel(); }

  COperationPanel* GetOperationPanel()  { return  m_frame->GetGuiPanel()->GetOperationPanel(); }

  wxMenu* GetMenuFieldsTreeCtrl()  { return  m_frame->GetMenuFieldsTreeCtrl(); }
  wxMenu* GetMenuOperationTreeCtrl()  { return  m_frame->GetMenuOperationTreeCtrl(); }
  wxMenu* GetMenuDisplayTreeCtrl() { return m_frame->GetMenuDisplayTreeCtrl(); }

  CWorkspace* GetCurrentWorkspace();
  CWorkspaceDataset* GetCurrentWorkspaceDataset();
  CWorkspaceOperation* GetCurrentWorkspaceOperation();
  CWorkspaceFormula* GetCurrentWorkspaceFormula();
  CWorkspaceDisplay* GetCurrentWorkspaceDisplay();

private:
	std::string GetWorkspaceKey( const std::string &subKey );

public:

  bool CanDeleteDataset(const wxString& name, CStringArray* operationNames = NULL);
  bool CanDeleteOperation(const wxString& name, CStringArray* displayNames = NULL);
  bool CanDeleteDisplay(const wxString& name);

  static bool RemoveFile(const wxString& name);
  static bool RenameFile(const wxString& oldName, const wxString& newName);

  int32_t GotoLastPageReached();
  int32_t GotoPage(int32_t pos);
  int32_t GotoPage(const wxString& name);
  
  wxString GetCurrentPageText();

  bool IsCurrentLogPage();

  int32_t GotoDatasetPage() {return GotoPage(DATASETS_PAGE_NAME);}
  int32_t GotoOperationPage() {return GotoPage(OPERATIONS_PAGE_NAME);}
  int32_t GotoDisplayPage() {return GotoPage(DISPLAY_PAGE_NAME);}
  int32_t GotoLogPage() {return GotoPage(LOG_PAGE_NAME);}

  wxFileName* GetExecName() {return &m_execName;}
  
  wxString GetExecPathName(int32_t flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE) 
#ifdef __WXMAC__
                        {return (m_execName.IsRelative() ? 
                                 m_execName.GetPath(flags, format) + "/BratGui.app/Contents/MacOS" :
                                 m_execName.GetPath(flags, format));}
#else
                        {return m_execName.GetPath(flags, format);}
#endif

  CTreeWorkspace* GetCurrentTree() {return m_currentTree;}
  void SetCurrentTree(CTreeWorkspace* value) {m_currentTree = value;}

  wxString GetLastColorTable() {return m_lastColorTable;}
  void SetLastColorTable(const wxString& value) {m_lastColorTable = value;}

  wxString BuildUserManualPath();
  void ViewUserManual();

  wxString GetUserManual() {return m_userManual;}
  void SetUserManual(const wxString& value) {m_userManual = value;}

  wxString GetLastDataPath() {return m_lastDataPath;}
  void SetLastDataPath(const wxString& value) {m_lastDataPath = value;}
  
  wxString GetLastPageReached() {return m_lastPageReached;}
  void SetLastPageReached(const wxString& value) {m_lastPageReached = value;}

  static void CStringArrayToWxArray(CStringArray& from, wxArrayString& to);
  static void CStringListToWxArray(const CStringList& from, wxArrayString& to);
  static void CProductListToWxArray(CProductList& from, wxArrayString& to);
  
  static wxString FindFile(const wxString& fileName );

  bool IsNewViewEnable();
  bool IsNewDatasetEnable();
  bool IsNewOperationEnable();
  
  bool IsDeleteViewEnable();
  bool IsDeleteDatasetEnable();
  bool IsDeleteOperationEnable();


  void GetRecordNames( wxArrayString& array, CProduct* product);
  void GetRecordNames( CStringArray& array, CProduct* product );
  void GetRecordNames(wxComboBox& combo, CProduct* product);
  void GetRecordNames(wxListBox& list, CProduct* product);

  static void EvtFocus(wxWindow& window,  int32_t eventType, const wxFocusEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtChar(wxWindow& window,  int32_t eventType, const wxCharEventFunction& method,
                        wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


public:
  CTreeWorkspace m_tree;
  CTreeWorkspace m_treeImport;

protected:


private:
  CTreeWorkspace* m_currentTree = nullptr;

  wxFileConfig* m_config = nullptr;

  wxString m_userManualViewer;
  wxString m_userManual;
  wxString m_lastWksPath;
  wxString m_lastColorTable;
  wxString m_lastDataPath;
  wxString m_lastPageReached;

  CGuiFrame* m_frame = nullptr;

  wxFileName m_execName;
};

DECLARE_APP(CBratGuiApp)

#endif
