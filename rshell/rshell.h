/**-----------------------------------------------------------------------------
 * Copyright (c) 2025 Sergey Sanders
 * sergey@sesadesign.com
 * -----------------------------------------------------------------------------
 * Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0
 * International (CC BY-NC-SA 4.0). 
 * 
 * You are free to:
 *  - Share: Copy and redistribute the material.
 *  - Adapt: Remix, transform, and build upon the material.
 * 
 * Under the following terms:
 *  - Attribution: Give appropriate credit and indicate changes.
 *  - NonCommercial: Do not use for commercial purposes.
 *  - ShareAlike: Distribute under the same license.
 * 
 * DISCLAIMER: This work is provided "as is" without any guarantees. The authors
 * aren’t responsible for any issues, damages, or claims that come up from using
 * it. Use at your own risk!
 * 
 * Full license: http://creativecommons.org/licenses/by-nc-sa/4.0/
 * ---------------------------------------------------------------------------*/

#ifndef _RSHELL_INCLUDED
#define _RSHELL_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#define MAX_PARAM_ARGUMENTS 16
#define CMD_NO_ERR          NULL
#define CMD_ERR_EMPTY       ""
#define CMD_MISSING_PARAM   "Missing parameter!"
#define CMD_MISSING_VALUE   "Missing value!"
#define CMD_UNKNOWN_OPTION  "Unknown option!"

typedef char* cmd_err_t;

typedef struct
{
	uint8_t	argc;
	char *argv[MAX_PARAM_ARGUMENTS];
} _cl_param_t;

typedef struct
{
    const char* name;
    const char* desc;
    cmd_err_t (*func)(_cl_param_t *);
} _cmd_t;

typedef struct
{
    const char *name;   // interface name
    const char *prompt; // interface prompt. 
    const char *desc;   // Short interface description
    bool (*init)(bool); // Interface initilaization function
    const _cmd_t cmdList[];
} _iface_t;

bool add_interface(const _iface_t *iface);
void rshell_task(void *vParam);
bool exec_line(char *str);
bool dump8(uint16_t addr, uint8_t *data,uint8_t size,uint8_t *lineNum);

/// Application greeting string. Compiled date and time will be printed if set to NULL
extern char *AppGreeting;
/// ucosR version calculated as VERSION * 100 + SUBVERSION
extern const uint16_t ucosRVersion; 
/// override weak interface_register() with a function consisting list of additional add_interface() calls 
void interface_register(void);

#endif //_RSHELL_INCLUDED