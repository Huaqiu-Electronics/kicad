#ifndef KI_SDK_SCH_UTILS_H
#define KI_SDK_SCH_UTILS_H


#include <cpp_sdk/ki_sdk_result_codes.h>
#include <cpp_sdk/api_cpp_sdk_param.h>
#include <sch_edit_frame.h>

#include <math/vector2d.h>
#include <symbol_edit_frame.h>
#include <base_units.h>


int SchMMToIU(const double& mm);

int SchMilsToIU( const int& mils );

double SchIUToMM( const int& iu );

int SchIUToMils( const int& iu );


VECTOR2I SchMMToIU( const API_POINT& pt );

API_POINT SchIUToMM( const VECTOR2I& pt );



KI_SDK_EXEC_RESULT GetSchEditorFrame( SCH_EDIT_FRAME*& frame );

KI_SDK_EXEC_RESULT      GetSchematic(SCHEMATIC*& schematic);

KI_SDK_EXEC_RESULT      GetSymbolFrame( SYMBOL_EDIT_FRAME*& frame );

KI_SDK_EXEC_RESULT     GetGlobalSymbolDir( std::string& symbol_dir);

#endif