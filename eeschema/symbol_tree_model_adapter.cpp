/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Chris Pavlina <pavlina.chris@gmail.com>
 * Copyright (C) 2014 Henner Zeller <h.zeller@acm.org>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "symbol_tree_model_adapter.h"

#include <wx/log.h>
#include <wx/tokenzr.h>
#include <wx/window.h>
#include <core/kicad_algo.h>
#include <pgm_base.h>
#include <project/project_file.h>
#include <widgets/wx_progress_reporters.h>
#include <dialogs/html_message_box.h>
#include <generate_alias_info.h>
#include <sch_base_frame.h>
#include <locale_io.h>
#include <symbol_async_loader.h>
#include <symbol_lib_table.h>
#include <string_utils.h>
#include <wildcards_and_files_ext.h>
#include <widgets/footprint_preview_widget.h>
#include <project_sch.h>
#include <gestfich.h>
#include "sch_field.h"
#include <confirm.h>


bool SYMBOL_TREE_MODEL_ADAPTER::m_show_progress = true;

#define PROGRESS_INTERVAL_MILLIS 33      // 30 FPS refresh rate


wxObjectDataPtr<LIB_TREE_MODEL_ADAPTER>
SYMBOL_TREE_MODEL_ADAPTER::Create( SCH_BASE_FRAME* aParent, LIB_TABLE* aLibs )
{
    auto* adapter = new SYMBOL_TREE_MODEL_ADAPTER( aParent, aLibs );
    return wxObjectDataPtr<LIB_TREE_MODEL_ADAPTER>( adapter );
}


SYMBOL_TREE_MODEL_ADAPTER::SYMBOL_TREE_MODEL_ADAPTER( SCH_BASE_FRAME* aParent, LIB_TABLE* aLibs ) :
        LIB_TREE_MODEL_ADAPTER( aParent, "pinned_symbol_libs", aParent->GetViewerSettingsBase() ),
        m_libs( (SYMBOL_LIB_TABLE*) aLibs )
{
    // Symbols may have different value from name
    m_availableColumns.emplace_back( wxT( "Value" ) );
}


SYMBOL_TREE_MODEL_ADAPTER::~SYMBOL_TREE_MODEL_ADAPTER()
{}


bool SYMBOL_TREE_MODEL_ADAPTER::AddLibraries( const std::vector<wxString>& aNicknames,
                                              SCH_BASE_FRAME* aFrame )
{
    std::unique_ptr<WX_PROGRESS_REPORTER> progressReporter = nullptr;

    if( m_show_progress )
    {
        progressReporter = std::make_unique<WX_PROGRESS_REPORTER>( aFrame,
                                                                   _( "Loading Symbol Libraries" ),
                                                                   aNicknames.size(), true );
    }

    // Disable KIID generation: not needed for library parts; sometimes very slow
    KIID::CreateNilUuids( true );

    std::unordered_map<wxString, std::vector<LIB_SYMBOL*>> loadedSymbolMap;

    SYMBOL_ASYNC_LOADER loader( aNicknames, m_libs, GetFilter() != nullptr, &loadedSymbolMap,
                                progressReporter.get() );

    LOCALE_IO toggle;

    loader.Start();

    while( !loader.Done() )
    {
        if( progressReporter && !progressReporter->KeepRefreshing() )
            break;

        wxMilliSleep( PROGRESS_INTERVAL_MILLIS );
    }

    loader.Join();

    bool cancelled = false;

    if( progressReporter )
        cancelled = progressReporter->IsCancelled();

    if( !loader.GetErrors().IsEmpty() )
    {
        HTML_MESSAGE_BOX dlg( aFrame, _( "Load Error" ) );

        dlg.MessageSet( _( "Errors loading symbols:" ) );

        wxString msg = loader.GetErrors();
        msg.Replace( "\n", "<BR>" );

        dlg.AddHTML_Text( msg );
        dlg.ShowModal();
    }

    if( loadedSymbolMap.size() > 0 )
    {
        COMMON_SETTINGS* cfg = Pgm().GetCommonSettings();
        PROJECT_FILE&    project = aFrame->Prj().GetProjectFile();

        auto addFunc =
                [&]( const wxString& aLibName, const std::vector<LIB_SYMBOL*>& aSymbolList,
                     const wxString& aDescription )
                {
                    std::vector<LIB_TREE_ITEM*> treeItems( aSymbolList.begin(), aSymbolList.end() );
                    bool pinned = alg::contains( cfg->m_Session.pinned_symbol_libs, aLibName )
                                  || alg::contains( project.m_PinnedSymbolLibs, aLibName );

                    DoAddLibrary( aLibName, aDescription, treeItems, pinned, false );
                };

        for( const auto& [libNickname, libSymbols] : loadedSymbolMap )
        {
            SYMBOL_LIB_TABLE_ROW* row = m_libs->FindRow( libNickname );

            wxCHECK2( row, continue );

            if( !row->GetIsVisible() )
                continue;

            std::vector<wxString> additionalColumns;
            row->GetAvailableSymbolFields( additionalColumns );

            for( const wxString& column : additionalColumns )
                addColumnIfNecessary( column );

            if( row->SupportsSubLibraries() )
            {
                std::vector<wxString> subLibraries;
                row->GetSubLibraryNames( subLibraries );

                wxString parentDesc = m_libs->GetDescription( libNickname );

                for( const wxString& lib : subLibraries )
                {
                    wxString suffix = lib.IsEmpty() ? wxString( wxT( "" ) )
                                                    : wxString::Format( wxT( " - %s" ), lib );
                    wxString name = wxString::Format( wxT( "%s%s" ), libNickname, suffix );
                    wxString desc = row->GetSubLibraryDescription( lib );

                    if( !parentDesc.IsEmpty() )
                    {
                        desc = wxString::Format( wxT( "%s (%s)" ),
                                                 parentDesc,
                                                 desc.IsEmpty() ? lib : desc );
                    }

                    UTF8 utf8Lib( lib );

                    std::vector<LIB_SYMBOL*> symbols;

                    std::copy_if( libSymbols.begin(), libSymbols.end(),
                                  std::back_inserter( symbols ),
                                  [&utf8Lib]( LIB_SYMBOL* aSym )
                                  {
                                      return utf8Lib == aSym->GetLibId().GetSubLibraryName();
                                  } );

                    addFunc( name, symbols, desc );
                }
            }
            else
            {
                addFunc( libNickname, libSymbols, m_libs->GetDescription( libNickname ) );
            }
        }
    }

    KIID::CreateNilUuids( false );

    m_tree.AssignIntrinsicRanks();

    if( progressReporter )
    {
        // Force immediate deletion of the APP_PROGRESS_DIALOG.  Do not use Destroy(), or Destroy()
        // followed by wxSafeYield() because on Windows, APP_PROGRESS_DIALOG has some side effects
        // on the event loop manager.
        // One in particular is the call of ShowModal() following SYMBOL_TREE_MODEL_ADAPTER
        // creating a APP_PROGRESS_DIALOG (which has incorrect modal behaviour).
        progressReporter.reset();
        m_show_progress = false;
    }

    return !cancelled;
}


void SYMBOL_TREE_MODEL_ADAPTER::AddLibrary( wxString const& aLibNickname, bool pinned )
{
    bool                        onlyPowerSymbols = ( GetFilter() != nullptr );
    std::vector<LIB_SYMBOL*>    symbols;
    std::vector<LIB_TREE_ITEM*> comp_list;

    try
    {
        m_libs->LoadSymbolLib( symbols, aLibNickname, onlyPowerSymbols );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Error loading symbol library '%s'." ) + wxS( "\n%s" ),
                    aLibNickname,
                    ioe.What() );
        return;
    }

    if( symbols.size() > 0 )
    {
        comp_list.assign( symbols.begin(), symbols.end() );
        DoAddLibrary( aLibNickname, m_libs->GetDescription( aLibNickname ), comp_list, pinned,
                      false );
    }
}


wxString SYMBOL_TREE_MODEL_ADAPTER::GenerateInfo( LIB_ID const& aLibId, int aUnit )
{
    return GenerateAliasInfo( m_libs, aLibId, aUnit );
}

void SYMBOL_TREE_MODEL_ADAPTER::InitConnection( const HTTP_HQ_LIB_SOURCE& aSource )
{
    if( !m_conn )
    {
        m_conn = std::make_shared<HTTP_HQ_CONNECTION>( aSource );
    }

}
bool SYMBOL_TREE_MODEL_ADAPTER::RequestCategories()
{
    HTTP_HQ_LIB_SOURCE src;
    src.root_url = m_hqRootUrl;
    InitConnection( src );
    
    if( !m_conn->RequestCategories() )
    {
        m_conn.reset();
        return false;
    }

    m_categories = m_conn->getCategories();
    return true;
}

bool SYMBOL_TREE_MODEL_ADAPTER::RequestQueryParts( const std::string& aCateId,
                            const std::string& aCateDisplayName,
                            const std::string& aDesc, const std::string& aPageNum,
                            const std::string& aPageSize )
{
    HTTP_HQ_LIB_SOURCE src;
    src.root_url = m_hqRootUrl;
    InitConnection( src );

    std::vector<std::pair<std::string, std::string>> fields;
    fields.push_back( std::make_pair( "cateId", aCateId ) );
    fields.push_back( std::make_pair( "categoryName", aCateDisplayName ) );
    fields.push_back( std::make_pair( "desc", aDesc ) );
    fields.push_back( std::make_pair( "pageNum", aPageNum ) );
    fields.push_back( std::make_pair( "pageSize", aPageSize ) );

    if( !m_conn->QueryParts( fields ) )
    {
        return false;
    }

    m_query_cache_parts = m_conn->getParts();

    for( auto part : m_conn->getParts() )
    {
        m_mpn_part_map[part.mpn] = part;
    }

    return true;
}

bool SYMBOL_TREE_MODEL_ADAPTER::RequestPartDetail( const std::string& aMpn )
{
    HTTP_HQ_LIB_SOURCE src;
    src.root_url = m_hqRootUrl;
    InitConnection( src );

    HTTP_HQ_PART& part = m_mpn_part_map.at( aMpn );


    if( !m_conn->RequestPartDetails( part ) )
    {
        return false;
    }

    // bool need_reload_tbl = false;
    // TODO: check if the file is expired, need a outdate flag
    /// only download when table do not have the symbol name
    // if( !m_libs->HasLibrary( part.symbol_lib_name ) 
    if( !m_conn->DownloadLibs( "symbol", part ) )
    {
        return false;
    }

    // NOTE: footprint table not LoadFileToInserterRow here, as KiCad construct FP_LIB_TABLE 
    // should not be included in eeschema here. It use kiway.
    // Should consider differrnt parts with same symbol lib file but not same fp lib file.
    if( !m_conn->DownloadLibs( "footprint", part ) )
    {
        return false;
    }

    return true;
}

bool SYMBOL_TREE_MODEL_ADAPTER::UpdateHQSymbolLib( const std::string& aMpn )
{
    SYMBOL_LIB_TABLE::LoadHQGlobalTable( SYMBOL_LIB_TABLE::GetHQGlobalLibTable() );

    // update HQ symbol lib file of lib name, symbol name, and some fields
    if( !SaveHQSymbolFields( aMpn ) )
        return false;

    SYMBOL_LIB_TABLE::LoadHQGlobalTable( SYMBOL_LIB_TABLE::GetHQGlobalLibTable() );
    return true;
}

bool SYMBOL_TREE_MODEL_ADAPTER::MoveHQLibsToPrjLibs( const wxString& aMpn,
                         SCH_BASE_FRAME* aFrame, FOOTPRINT_PREVIEW_WIDGET* aWidget )
{
    if( m_mpn_part_map.find( aMpn ) == m_mpn_part_map.end() )
    {
        wxLogError( _( "Error loading HQ part to move libs to project." ) );
        return false;
    }
    HTTP_HQ_PART& part = m_mpn_part_map.at( aMpn );

    SYMBOL_LIB_TABLE_ROW* row = m_libs->FindRow( part.symbol_lib_name );
    SYMBOL_LIB_TABLE* sym_table = PROJECT_SCH::SchSymbolLibTable( &aFrame->Prj() );

    if( row )
    {
        // copy symbol to prj hq_libs
        wxFileName symfn = m_conn->GetLibSavePath( "symbol", part );
        wxFileName fpfn = m_conn->GetLibSavePath( "footprint", part );
        wxFileName prjfn;
        prjfn.AssignDir( aFrame->Prj().GetProjectDirectory() );
        prjfn.AppendDir( "hq_libs" );
        prjfn.SetFullName( symfn.GetFullName() );

        if( !prjfn.FileExists() )
        {
            if( !prjfn.DirExists() && !prjfn.Mkdir( 0x777, wxPATH_MKDIR_FULL ) )
            {
                THROW_IO_ERROR( wxString::Format( _( "Cannot create project hq_libs path '%s'." ),
                                                prjfn.GetPath() ) );
            }
        }

        wxString msg;

        KiCopyFile( symfn.GetFullPath(), prjfn.GetFullPath(), msg );
        if( !msg.IsEmpty() )
        {
            DisplayError( aFrame, wxString::Format( _( "Error saving hq lib files '%s'." ),
                                                prjfn.GetFullPath() ) );
            return false;
        }

        wxString fp_pretty = wxString::Format( "%s.%s", part.pretty_name,
                FILEEXT::KiCadFootprintLibPathExtension );

        prjfn.AppendDir( fp_pretty );

        prjfn.SetFullName( fpfn.GetFullName() );

        if( !prjfn.FileExists() )
        {
            if( !prjfn.DirExists() && !prjfn.Mkdir( 0x777, wxPATH_MKDIR_FULL ) )
            {
                THROW_IO_ERROR( wxString::Format( _( "Cannot create project hq_libs path '%s'." ),
                                                prjfn.GetPath() ) );
            }
        }

        KiCopyFile( fpfn.GetFullPath(), prjfn.GetFullPath(), msg );
        if( !msg.IsEmpty() )
        {
            DisplayError( aFrame, wxString::Format( _( "Error saving hq lib files '%s'." ),
                                                prjfn.GetFullPath() ) );
            return false;
        }

        // to insert prj table row
        wxString uri = wxS( "${KIPRJMOD}/hq_libs/" ) + symfn.GetFullName();
        wxString fp_uri = wxString::Format( "${KIPRJMOD}/hq_libs/%s", fp_pretty );

        bool ret = aWidget->UpdateHQPrjFPLibTable( fp_uri, part.pretty_name );
        // the row was inserted before, not need to insert again,
        // just overwrite the libs files and update
        if( sym_table->HasLibraryWithPath( uri ) && ret )
            return true;

        wxString libNickname = symfn.GetName();

        SYMBOL_LIB_TABLE_ROW* add_row = new SYMBOL_LIB_TABLE_ROW( libNickname, uri, wxT( "KiCad" ), wxEmptyString,
                                                _( "Added by HQ Online Symbol" ) );
        sym_table->InsertRow( add_row, true );

        sym_table->Save( aFrame->Prj().SymbolLibTableName() );

        // try save 
        aFrame->SavePrjSymbolLibTables();

        return true;
    }

    return false;
}

bool SYMBOL_TREE_MODEL_ADAPTER::SaveHQSymbolFields( const std::string& aMpn )
{
    if( m_mpn_part_map.find( aMpn ) == m_mpn_part_map.end() )
    {
        wxLogError( _( "Error loading HQ symbol to update tree item." ) );
        return false;
    }
    HTTP_HQ_PART& part = m_mpn_part_map.at( aMpn );
    std::vector<LIB_SYMBOL*>    symbols;
    wxString lib_nick_name = part.symbol_lib_name;
    // wxString nickname = libname.substr( 0, libname.length() - 10 );

    try
    {
        m_libs->LoadSymbolLib( symbols, lib_nick_name, false );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Error loading HQ symbol library '%s'." ) + wxS( "\n%s" ),
                    lib_nick_name,
                    ioe.What() );
        return false;
    }

    LIB_SYMBOL* lib_sym = symbols.at( 0 );
    wxString old_symName = lib_sym->GetName();
    lib_sym->SetName( lib_nick_name );
    std::vector<SCH_FIELD> fields;
    int field_id = 5;
    // TODO: to change logic for situation lib_sym fields don't has not MANDATORY_FIELDS in order.
    // Set fields order by manual.
    if( part.attrs.find( "Mpn" ) != part.attrs.end() )
    {
        SCH_FIELD* field = new SCH_FIELD( lib_sym, field_id++, "Mpn" );
        // field->SetId(  );
        // field->SetName();
        field->SetShowInChooser( false );
        field->SetVisible( false );
        field->SetText( part.attrs["Mpn"] );
        fields.push_back( *field );
    }

    if( part.attrs.find( "Manufacturer" ) != part.attrs.end() )
    {
        SCH_FIELD* field = new SCH_FIELD( lib_sym, field_id++, "Manufacturer" );
        // field->SetId( field_id++ );
        // field->SetName( "Manufacturer" );
        field->SetShowInChooser( false );
        field->SetVisible( false );
        field->SetText( part.attrs["Manufacturer"] );
        fields.push_back( *field );
    }

    for( auto& [name, text] : part.attrs )
    {
        SCH_FIELD* field = nullptr;
        if( name == "Value" )
        {
            field = new SCH_FIELD( lib_sym->GetValueField() );
            field->SetVisible( false );
        }
        else if( name == "Footprint" )
        {
            field = new SCH_FIELD( lib_sym->GetFootprintField() );
            field->SetShowInChooser( false );
            field->SetVisible( false );
        }
        else if( name == "Datasheet" )
        {
            field = new SCH_FIELD( lib_sym->GetDatasheetField() );
            field->SetShowInChooser( false );
            field->SetVisible( false );
        }
        else if( name == "Description" )
        {
            field = new SCH_FIELD( lib_sym->GetDescriptionField() );
            field->SetShowInChooser( false );
            field->SetVisible( false );
        }
        else if( name != "Manufacturer" && name != "Mpn" )
        {
            field = new SCH_FIELD( lib_sym, field_id++, name );
            // field->SetId( field_id++ );
            // field->SetName( name );
            field->SetShowInChooser( false );
            field->SetVisible( false );
        }

        if( name != "Manufacturer" && name != "Mpn" )
        {
            field->SetText( text );
            fields.push_back( *field );
        }
    }
    SCH_FIELD* ref_field = new SCH_FIELD( lib_sym->GetReferenceField() );
    ref_field->SetShowInChooser( true );
    ref_field->SetVisible( true );
    fields.push_back( *ref_field );
    // replace old fields
    lib_sym->SetHqPartsFields( fields );

    wxFileName fn( m_conn->GetLibSavePath( "symbol", part ) );

    try
    {
        IO_RELEASER<SCH_IO> pi( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_KICAD ) );
        
        pi->DeleteSymbol( fn.GetFullPath(), old_symName );

        pi->SaveSymbol( fn.GetFullPath(), new LIB_SYMBOL( *lib_sym ) );

    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Failed to save HQ library %s. %s" ), fn.GetFullPath(), ioe.What() );
        return false;
    }

    return true;
}


void SYMBOL_TREE_MODEL_ADAPTER::AddHQPartsToLibraryNode( LIB_TREE_NODE_LIBRARY& aNode, bool pinned )
{
    std::vector<LIB_SYMBOL*>    symbols;
    std::vector<LIB_TREE_ITEM*> comp_list;
    wxString name;

    try
    {
        for( auto& part : m_query_cache_parts )
        {
            std::unique_ptr<LIB_SYMBOL> symbol = std::make_unique<LIB_SYMBOL>( part.mpn );

            symbol->SetUnitCount( 1 );

            name = wxString::FromUTF8( symbol->GetName().c_str() );

            // Some symbol LIB_IDs have the '/' character escaped which can break derived symbol links.
            // The '/' character is no longer an illegal LIB_ID character so it doesn't need to be
            // escaped.
            name.Replace( wxS( "{slash}" ), wxT( "/" ) );

            LIB_ID id;
            int bad_pos = id.Parse( name );

            if( bad_pos >= 0 )
            {
                if( static_cast<int>( name.size() ) > bad_pos )
                {
                    wxString msg = wxString::Format(
                            _( "Symbol %s contains invalid character '%c'" ), name,
                            name[bad_pos] );

                }
            }

            symbol->SetName( id.GetLibItemName().wx_str() );
            symbol->SetLibId( id );

            // update item description before click to query part details
            symbol->SetDescription( part.description );

            // m_libsymbol_part_map[symbol->GetName()] = part;
            symbols.push_back( symbol.release() );
        }
        
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Error loading HQ symbol of '%s'." ) + wxS( "\n%s" ),
                    name, ioe.What() );
        return;
    }

    if( symbols.size() > 0 )
    {
        if( symbols.size() < 10 )
            symbols.push_back( new LIB_SYMBOL( "-- No more results --" ) );
        else
            symbols.push_back( new LIB_SYMBOL( "-- More results --" ) );  
    }
    else
    {
        symbols.push_back( new LIB_SYMBOL( "-- No more results --" ) );
    }
    
    comp_list.assign( symbols.begin(), symbols.end() );
        
    AddItemToLibraryNode( aNode, comp_list, pinned, false );
}

void SYMBOL_TREE_MODEL_ADAPTER::UpdateTreeItemLibSymbol( LIB_TREE_NODE_ITEM* aItem )
{
    if( m_mpn_part_map.find( aItem->m_Name ) == m_mpn_part_map.end() )
    {
        wxLogError( _( "Error loading HQ symbol to update tree item." ) );
        return;
    }
    HTTP_HQ_PART& part = m_mpn_part_map.at( aItem->m_Name );

    std::vector<LIB_SYMBOL*>    symbols;
    std::vector<LIB_TREE_ITEM*> comp_list;
    wxString lib_nick_name = part.symbol_lib_name;

    try
    {
        m_libs->LoadSymbolLib( symbols, lib_nick_name, false );
    }
    catch( const IO_ERROR& ioe )
    {
        wxLogError( _( "Error loading HQ symbol library '%s'." ) + wxS( "\n%s" ),
                    lib_nick_name,
                    ioe.What() );
        return;
    }

    if( symbols.size() > 0 )
    {
        comp_list.assign( symbols.begin(), symbols.end() );
        
        for( LIB_TREE_ITEM* item: comp_list )
        {
            aItem->Update( item );
            // for some mpn name may be illegal for lib name,
            // they will not be equal
            aItem->m_Name = part.mpn;
        }
    }
}



void SYMBOL_TREE_MODEL_ADAPTER::LoadCategories()
{
    if( !m_categories.empty() )
    {
        m_name_category_map.clear();
        std::vector<HTTP_HQ_CATEGORY> level1_vec;
        std::vector<HTTP_HQ_CATEGORY> level2_vec;
        std::vector<HTTP_HQ_CATEGORY> level3_vec;
        for( auto& category : m_categories )
        {
            if( category.level == "1" )
            {
                level1_vec.push_back( category );

            }
            else if( category.level == "2" )
            {
                level2_vec.push_back( category );

            }
            else if( category.level == "3" )
            {
                level3_vec.push_back( category );
            }
            m_name_category_map[category.displayName] = category;
        }

        for( auto& level1  : level1_vec )
        {
            LIB_TREE_NODE_LIBRARY& level1_node = 
                AddSubLibraryNode( level1.displayName, wxEmptyString, false);

            for( auto& level2  : level2_vec )
            {
                if( level1.id == level2.parentId )
                {
                    LIB_TREE_NODE_LIBRARY& level2_node =
                        AddSubLibraryNode( level1_node, level2.displayName,
                                                       wxEmptyString, false);
                    for( auto& level3  : level3_vec )
                    {
                        if( level2.id == level3.parentId )
                        {
                            // LIB_TREE_NODE_LIBRARY& level3_node =
                            AddSubLibraryNode( level2_node, level3.displayName,
                                                            wxEmptyString, false);
                        }
                    }
                }
            }

            level1_node.AssignIntrinsicRanks( true );
        }
    }
}

HTTP_HQ_CATEGORY SYMBOL_TREE_MODEL_ADAPTER::GetHQCategory( const wxString& aDisplayname )
{
    if ( m_name_category_map.find( aDisplayname ) != m_name_category_map.end() )
    {
        return m_name_category_map[aDisplayname];
    }
    else
    {
        return HTTP_HQ_CATEGORY();
    }
}

