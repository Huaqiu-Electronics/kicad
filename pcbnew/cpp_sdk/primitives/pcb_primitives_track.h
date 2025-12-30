#ifndef PCB_PRIMITIVES_TRACK_H
#define PCB_PRIMITIVES_TRACK_H


#include <math/vector2d.h>
#include <string>
#include <vector>

enum PCB_LAYER_ID : int;

enum class KI_SDK_EXEC_RESULT : int;
class PCB_PRIMITIVES_TRACK
{
public:
    KI_SDK_EXEC_RESULT CreateTrack( int netCode, const VECTOR2I& start, const VECTOR2I& end, PCB_LAYER_ID layer,
                                    int lineWidth, bool lock, std::string& aKID );

    KI_SDK_EXEC_RESULT CreateArcTrack( int netCode, const VECTOR2I& start, const VECTOR2I& mid, const VECTOR2I& end,
                                       PCB_LAYER_ID layer, int lineWidth, bool lock, std::string& aKID );

    KI_SDK_EXEC_RESULT GetAllTracksKIID( std::vector<std::string>& allKIDList );


    KI_SDK_EXEC_RESULT GetTrackWidth( const std::string& aKID, int& width );

    KI_SDK_EXEC_RESULT GetTrackLayer( const std::string& aKID, PCB_LAYER_ID& layer );


    KI_SDK_EXEC_RESULT GetTrackLayerName( const std::string& aKID, std::string& layerName );


    KI_SDK_EXEC_RESULT GetTrackNetCode( const std::string& aKID, int& netCode );


    KI_SDK_EXEC_RESULT GetTrackNetName( const std::string& aKID, std::string& netName );


    KI_SDK_EXEC_RESULT GetTrackStart( const std::string& aKID, VECTOR2I& start );


    KI_SDK_EXEC_RESULT GetTrackEnd( const std::string& aKID, VECTOR2I& end );


    KI_SDK_EXEC_RESULT GetTrackMid( const std::string& aKID, VECTOR2I& mid );


    KI_SDK_EXEC_RESULT GetTrackLockState( const std::string& aKID, bool& lock );


    KI_SDK_EXEC_RESULT SetTrackNetCode( const std::string& aKID, int netCode );


    KI_SDK_EXEC_RESULT SetTrackWidth( const std::string& aKID, int width );


    KI_SDK_EXEC_RESULT SetTrackLayer( const std::string& aKID, PCB_LAYER_ID layer );


    KI_SDK_EXEC_RESULT SetTrackStart( const std::string& aKID, const VECTOR2I& start );


    KI_SDK_EXEC_RESULT SetTrackEnd( const std::string& aKID, const VECTOR2I& end );


    KI_SDK_EXEC_RESULT SetTrackLockState( const std::string& aKID, bool lock );


    KI_SDK_EXEC_RESULT DeleteAllTracks();


    KI_SDK_EXEC_RESULT DeleteTrackByKIID( const std::string& aKID );
};


#endif