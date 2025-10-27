#ifndef FREEROUTING_OWNED_HANDLES_H
#define FREEROUTING_OWNED_HANDLES_H

#include <context/copilot_freerouting_handle.h>

struct FREEROUTING_OWNED_HDL
{
    COPILOT_DSN_OWNED_HDL           dsn_handle;
    COPILOT_SETJOBID_OWNER_HDL      set_job_id_handle;
    COPILOT_GETJOBID_OWNED_HDL      get_job_id_handle;
    COPILOT_SPECC_OWNER_HDL         specc_handle;
};

#endif