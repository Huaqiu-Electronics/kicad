/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016-2023 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */
#ifndef PANEL_HQ_SYMBOL_CHOOSER_H
#define PANEL_HQ_SYMBOL_CHOOSER_H

#include <widgets/lib_tree.h>
#include <symbol_tree_model_adapter.h>
#include <footprint_info.h>
#include <widgets/html_window.h>
#include <wx/srchctrl.h>
#include <widgets/std_bitmap_button.h>

class wxPanel;
class wxTimer;
class wxSplitterWindow;

class SYMBOL_PREVIEW_WIDGET;
class FOOTPRINT_PREVIEW_WIDGET;
class FOOTPRINT_SELECT_WIDGET;
class SCH_BASE_FRAME;
struct PICKED_SYMBOL;

class PANEL_HQ_SYMBOL_CHOOSER : public wxPanel
{
public:
/**
 * Create dialog to choose symbol.
 *
 * @param aFrame  the parent frame (usually a SCH_EDIT_FRAME or SYMBOL_CHOOSER_FRAME)
 * @param aParent the parent window (usually a DIALOG_SHIM or SYMBOL_CHOOSER_FRAME)
 * @param aAllowFieldEdits  if false, all functions that allow the user to edit fields
 *                          (currently just footprint selection) will not be available.
 * @param aShowFootprints   if false, all footprint preview and selection features are
 *                          disabled. This forces aAllowFieldEdits false too.
 * @param aAcceptHandler a handler to be called on double-click of a footprint
 * @param aEscapeHandler a handler to be called on <ESC>
 */
    PANEL_HQ_SYMBOL_CHOOSER( SCH_BASE_FRAME* aFrame, wxWindow* aParent,
                          const SYMBOL_LIBRARY_FILTER* aFilter,
                          std::vector<PICKED_SYMBOL>& aHistoryList,
                          std::vector<PICKED_SYMBOL>& aAlreadyPlaced,
                          bool aAllowFieldEdits, bool aShowFootprints,
                          std::function<void()> aAcceptHandler,
                          std::function<void()> aEscapeHandler );


    virtual ~PANEL_HQ_SYMBOL_CHOOSER();

    void OnChar( wxKeyEvent& aEvent );

    void FinishSetup( wxWindow* aParent );

    void SetPreselect( const LIB_ID& aPreselect );

    /**
     * To be called after this dialog returns from ShowModal().
     *
     * For multi-unit symbols, if the user selects the symbol itself rather than picking
     * an individual unit, 0 will be returned in aUnit.
     * Beware that this is an invalid unit number - this should be replaced with whatever
     * default is desired (usually 1).
     *
     * @param aUnit if not NULL, the selected unit is filled in here.
     * @return the #LIB_ID of the symbol that has been selected.
     */
    LIB_ID GetSelectedLibId( int* aUnit = nullptr ) const;

    int GetItemCount() const { return m_adapter->GetItemCount(); }

    wxWindow* GetFocusTarget() const { return m_tree->GetFocusTarget(); }

    /**
     * Get a list of fields edited by the user.
     *
     * @return vector of pairs; each.first = field ID, each.second = new value.
     */
    std::vector<std::pair<int, wxString>> GetFields() const
    {
        return m_field_edits;
    }

protected:
    static constexpr int DBLCLICK_DELAY = 100; // milliseconds

    wxPanel* constructRightPanel( wxWindow* aParent );

    void OnDetailsCharHook( wxKeyEvent& aEvt );
    void onCloseTimer( wxTimerEvent& aEvent );
    void onOpenLibsTimer( wxTimerEvent& aEvent );

    void onFootprintSelected( wxCommandEvent& aEvent );
    virtual void onSymbolSelected( wxCommandEvent& aEvent );

    /**
     * Handle the selection of an item. This is called when either the search box or the tree
     * receive an Enter, or the tree receives a double click.
     * If the item selected is a category, it is expanded or collapsed; if it is a symbol, the
     * symbol is picked.
     */
    void onSymbolChosen( wxCommandEvent& aEvent );

    /**
     * Look up the footprint for a given symbol specified in the #LIB_ID and display it.
     */
    void showFootprintFor( const LIB_ID& aLibId );

    /**
     * Display the given footprint by name.
     */
    void showFootprint( const wxString& aFootprint );

    /**
     * Populate the footprint selector for a given alias.
     *
     * @param aLibId the #LIB_ID of the selection or invalid to clear.
     */
    void populateFootprintSelector( const LIB_ID& aLibId );

public:
    static std::mutex g_Mutex;

protected:
    static wxString           g_symbolSearchString;
    static wxString           g_powerSearchString;

    wxTimer*                  m_dbl_click_timer;
    wxTimer*                  m_open_libs_timer;
    SYMBOL_PREVIEW_WIDGET*    m_symbol_preview;
    wxSplitterWindow*         m_hsplitter;
    wxSplitterWindow*         m_vsplitter;

    wxObjectDataPtr<LIB_TREE_MODEL_ADAPTER> m_adapter;

    FOOTPRINT_SELECT_WIDGET*  m_fp_sel_ctrl;
    FOOTPRINT_PREVIEW_WIDGET* m_fp_preview;
    LIB_TREE*                 m_tree;
    HTML_WINDOW*              m_details;

    SCH_BASE_FRAME*           m_frame;
    std::function<void()>     m_acceptHandler;
    std::function<void()>     m_escapeHandler;

    bool                      m_showPower;
    bool                      m_allow_field_edits;
    bool                      m_show_footprints;
    wxString                  m_fp_override;

    std::vector<std::pair<int, wxString>>  m_field_edits;
};


class HQ_LIB_TREE : public LIB_TREE
{
public:
    HQ_LIB_TREE( wxWindow* aParent, const wxString& aRecentSearchesKey, LIB_TABLE* aLibTable,
              wxObjectDataPtr<LIB_TREE_MODEL_ADAPTER>& aAdapter, int aFlags = ALL_WIDGETS,
              HTML_WINDOW* aDetails = nullptr )
       : LIB_TREE( aParent, aRecentSearchesKey, aLibTable, aAdapter, aFlags, aDetails )
    {
        if( m_query_ctrl )
        {
            m_query_ctrl->Unbind( wxEVT_TEXT, &LIB_TREE::OnQueryText, this );
            m_query_ctrl->Unbind( wxEVT_SEARCH_CANCEL, &LIB_TREE::OnQueryText, this );   
        }
        m_query_ctrl->ShowCancelButton( false );
        m_query_ctrl->Bind( wxEVT_TEXT, &HQ_LIB_TREE::QueryText, this );
        m_query_ctrl->Bind( wxEVT_SEARCH_CANCEL, &HQ_LIB_TREE::QueryText, this );

        if( m_sort_ctrl )
            delete m_sort_ctrl;

    }

    ~HQ_LIB_TREE()
    {
        m_query_ctrl->Unbind( wxEVT_TEXT, &HQ_LIB_TREE::QueryText, this );
        m_query_ctrl->Unbind( wxEVT_SEARCH_CANCEL, &HQ_LIB_TREE::QueryText, this );

    }

protected:
    /**
     * Regenerate the tree.
     */

    void QueryText( wxCommandEvent& aEvent )
    {
        wxString filter = m_query_ctrl->GetValue();
        SYMBOL_TREE_MODEL_ADAPTER* adapter = static_cast<SYMBOL_TREE_MODEL_ADAPTER*>( m_adapter.get() );
        wxString hq_node = wxT( "-- HQ Online Search Results --" );

        wxDataViewItem item = adapter->FindItem( LIB_ID( hq_node, wxEmptyString ) );
        LIB_TREE_NODE_LIBRARY* http_node = nullptr;
        if( filter.IsEmpty() )
        {
            //to remove HQ HTTP node in m_tree.m_children if exist
            if( item.IsOk() )
            {
                http_node = static_cast<LIB_TREE_NODE_LIBRARY*>( adapter->GetTreeNodeFor( item ) );
                http_node->m_Children.clear();

                adapter->UpdateTreeAfterAddHQPart( http_node, true );
            }
            // postPreselectEvent();

        }
        else
        {
            std::string args = "";
            adapter->RequestQueryParts( args, args, filter.ToStdString() );
            //to check node in m_tree.m_children,if dont't exist,add it .
            //if exist, do not add twice
            
            if( !item.IsOk() )
            {
                http_node = &( adapter->AddSubLibraryNode( hq_node, wxEmptyString, false) );
            }
            else
            {
                http_node = static_cast<LIB_TREE_NODE_LIBRARY*>( adapter->GetTreeNodeFor( item ) );
                http_node->m_Children.clear(); 
            }
            adapter->AddHQPartsToLibraryNode( *http_node, true );

            adapter->UpdateTreeAfterAddHQPart( http_node, true );

        }
    }

public:

    virtual void UpdateSelectItem()
    {
        selectIfValid( m_tree_ctrl->GetSelection() );
    }

    wxString GetSearchString() const
    {
        return m_query_ctrl->GetValue();
    }

};

#endif /* PANEL_HQ_SYMBOL_CHOOSER */

