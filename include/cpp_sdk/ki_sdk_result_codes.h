#ifndef KI_SDK_RESULT_CODES_H
#define KI_SDK_RESULT_CODES_H


enum class KI_SDK_EXEC_RESULT : int
{
    KI_SDK_SUCCESS = 0, 

    KI_SDK_FAILURE = -1,           
    KI_SDK_INVALID_INPUT = -2,    
    KI_SDK_NOT_SUPPORTED = -3,    
    KI_SDK_NOT_FOUND = -4,       
    KI_SDK_PERMISSION_DENIED = -5, 
    KI_SDK_TIMEOUT = -6,     
    KI_SDK_INTERNAL_ERROR = -7,    
    KI_SDK_FRAME_ERROR = -8,       
    KI_SDK_BOARD_ERROR = -9,       
    KI_SDK_ALREADY_EXISTS = -10
};


#endif