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
/**
 * @file rshell.c
 * @author Sergey Sanders
 * @date September 2022-25
 * @brief * Rimer command line shell
 *
 */
#include "rshell.h"

#include "FreeRTOS.h"
#include "bsp.h"
#include "lcd.h"
#include "commandline.h"
#include "editline.h"
#include "ff.h"
#include "keyboard.h"
#include "llfs.h"
#include "sys_config.h"
#include "task.h"
#include "icons.h"
#include "graph.h"
#include "colours.h"
#include "tstring.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

_cl_param_t cliParam;

#define ACTIVE_IFACE 0
#define BUILTIN_IFACE 1

#define PATH_FILE_NAME ".path.list"

#define PIPED_PARAM_AT_END 1

typedef struct
{
   char *name;
   void *prev;
   FIL file;
} _script_t;

static struct
{
   char str[2][EL_LENGTH]; // double buffer
   uint8_t ptr;
   uint8_t active;
} pipeBuff = {0}; // Pipe buffer size.

void __attribute__((weak)) interface_registstration(void)
{
   /// list of add_interface(&ifaceStruct)
   /// calls to add relevant interfaces to the shell
}

static cmd_err_t shell_help(_cl_param_t *sParam);
static cmd_err_t shell_version(_cl_param_t *sParam);
static cmd_err_t shell_ifaces_list(_cl_param_t *sParam);
static cmd_err_t shell_echo(_cl_param_t *sParam);
static cmd_err_t shell_path(_cl_param_t *sParam);
static cmd_err_t shell_cls(_cl_param_t *sParam);
static cmd_err_t shell_rs(_cl_param_t *sParam);
static cmd_err_t shell_red(_cl_param_t *sParam);

#define RSHELL_DEBUG    1
#if RSHELL_DEBUG
static cmd_err_t shell_param(_cl_param_t *sParam);
static cmd_err_t shell_eefrmt(_cl_param_t *sParam);
static cmd_err_t shell_enum(_cl_param_t *sParam);
#endif

extern _iface_t ifaceSystem;
extern _iface_t ifaceUtil;

_iface_t ifaceShell = {
    .name = "/",
    .desc = "Rimer shell bult-in commands",
    .init = NULL,
    //
    .cmdList = {
        {.name = "help", .desc = "Show this help", .func = shell_help},
        {.name = "?", .desc = "Shortcut for help", .func = shell_help},
        {.name = "ver", .desc = "System version", .func = shell_version},
        {.name = "iface", .desc = "Show interfaces list", .func = shell_ifaces_list},
        {.name = "echo", .desc = "Print a value", .func = shell_echo},
        {.name = "path", .desc = "[a]dd/[d]elete/show interfaces path", .func = shell_path},
        {.name = "cls", .desc = "Clear screen", .func = shell_cls},
        {.name = "rs", .desc = "Run script form SD card", .func = shell_rs},
        {.name = "red", .desc = "Run red text editor", .func = shell_red},
#if RSHELL_DEBUG
        {.name = "enum", .desc = "Enum test", .func = shell_enum},
        {.name = "param", .desc = "print parameters", .func = shell_param},
        {.name = "eefrmt", .desc = "format EEPROM", .func = shell_eefrmt},
#endif
        {.name = NULL, .func = NULL},
    }};

typedef struct
{
   const _iface_t *iface;
   void *next;
} _iface_list_t;

static const _iface_t *execInterface;
static const _iface_t *get_interface(char *str);

static _iface_list_t *ifaceList = &((_iface_list_t){.iface = &ifaceSystem, .next = NULL});
static _iface_list_t *ifacePath = NULL;
static _script_t *script = NULL;

bool add_interface(const _iface_t *iface)
{
   _iface_list_t *list = ifaceList;
   while (list->next)
      list = (_iface_list_t *)list->next;
   list->next = pvPortMalloc(sizeof(_iface_list_t));
   if (!list->next)
      return false;
   list = list->next;
   list->iface = iface;
   list->next = NULL;
   return true;
}

const _iface_t *get_interface(char *str)
{
   _iface_list_t *list = ifaceList;
   while (1)
   {
      if (!strcmp(list->iface->name, str))
         return list->iface;
      if (list->next)
         list = (_iface_list_t *)list->next;
      else
         return NULL;
   }
}

/**
 * @brief  allocate/reallocate an array
 * @param   actInterface - active interface
 * @return  terminating pointer
 */
void init_iface_path(const _iface_t *actIface)
{
   if (ifacePath)
      return;
   ifacePath = pvPortMalloc(sizeof(_iface_list_t));
   ifacePath->next = pvPortMalloc(sizeof(_iface_list_t));
   ifacePath->iface = actIface ? actIface : &ifaceShell;
   ((_iface_list_t *)(ifacePath->next))->iface = &ifaceShell;
   ((_iface_list_t *)(ifacePath->next))->next = NULL;
   return;
}

const _iface_t *iface_in_path(char *str)
{
   _iface_list_t *path = ifacePath->next; // start from "/"
   while (1)
   {
      if (!strcmp(path->iface->name, str))
         return path->iface;
      if (path->next)
         path = (_iface_list_t *)path->next;
      else
         return NULL;
   }
}

/** Delete an interface from the path
 *  deletes all records except the first two:
 *  current interface and the root interface
 **/
bool iface_path_del(char *name)
{
   _iface_list_t *tmpPath = ifacePath->next;
   _iface_list_t *pathNext = tmpPath->next;
   if (!pathNext)
      return false;
   if (!name || !*name || *name == '*')
   {
      tmpPath->next = NULL; // only two basic paths left
      while (1)
      {
         vPortFree(pathNext);
         if (!(pathNext = pathNext->next))
            return true;
      }
   }
   while (1)
   {
      if (!strcmp(pathNext->iface->name, name))
      {
         tmpPath->next = pathNext->next;
         vPortFree(pathNext);
         return true;
      }
      tmpPath = pathNext;
      if (!(pathNext = pathNext->next))
         return false;
   }
}

/** Set comma separated interfaces list to search while executing the command line
 * First is always the selected interface
 * Second is always the shell built-in interface
 * upcoming is the list to search. Shorter the list - faster per-line execution.
 * */
bool iface_path_add(char *list)
{
   {
      _iface_list_t *tmpPath = (_iface_list_t *)ifacePath->next; // set to "/"
      _iface_t *tmpIface;
      char *name;

      if (!list)
         return true;
      while (tmpPath->next)
         tmpPath = tmpPath->next;
      name = strtok(list, ",\r\n");
      while (name)
      {
         if ((tmpIface = (_iface_t *)get_interface(name)) &&
             (!iface_in_path(name))) // inerface is valid and not in path yet
         {
            if (!(tmpPath->next = pvPortMalloc(sizeof(_iface_list_t))))
               return false;
            tmpPath = tmpPath->next;
            tmpPath->iface = tmpIface;
            tmpPath->next = NULL;
         }
         name = strtok(NULL, ",\r\n");
      }
      return true;
   }
}

static void _print_help(const _iface_t *iface)
{
   tprintf("\e[a\e[37m[ %s ]\n", iface->name); // save the color attribute and set color to 8
   for (uint8_t fcurPos = 0; iface->cmdList[fcurPos].name; fcurPos++)
   {
      tprintf("  %s - %s\n", iface->cmdList[fcurPos].name, iface->cmdList[fcurPos].desc);
   }
   tprintf("\e[r");
}

bool dump8(uint16_t addr, uint8_t *data, uint8_t size, uint8_t *lineNum)
{
   char askiiStr[9];
   uint8_t i;
   if (++*lineNum == 19)
   {
      tprintf("Press any key to continue");
      *lineNum = 0;
      if (keyboard_wait(" "))
      {
         tprintf("\r\e[0K");
         return false;
      }
   }
   tprintf("\r%4x: ", addr);
   for (i = 0; i < 8; i++)
   {
      if (i < size)
      {
         tprintf("%2x ", data[i]);
         askiiStr[i] = (data[i] >= ' ' && data[i] <= '~') ? data[i] : '.';
         askiiStr[i + 1] = '\0';
      }
      else
         tprintf("   ");
   }
   tprintf("|%s\n", askiiStr);
   return true;
}

/** -------------------- Command line interface and parser -------------------- **/

/// strtok with quoted string exception
char *qstrtok(char *str, char *delim)
{
   static char *ptr;
   char *tok;
   if (!str)
   {
      str = ptr;
      if (!str || !*str)
         return NULL;
   }
   while (*str && strchr(delim, *str))
      str++; // skip leading delimiter(s)
   if (!*str)
      return NULL;
   if (*str == '"')
   {
      tok = str + 1;
      while (*++str != '"') // find second quote
         if (!*str)         // terminating token
         {
            ptr = str;
            return tok;
         }
   }
   else
   {
      tok = str;
      while (*str && !strchr(delim, *str))
         str++;
      if (!*str) // terminating token
      {
         ptr = str;
         return tok;
      }
   }
   *str = '\0';
   ptr = str + 1;
   return tok;
}

/// sstrchr with quoted string exception
char *qstrchr(char *str, char srch)
{
   while (*str)
   {
      if (*str == srch)
         return str;
      if (*str == '"') // quoted expression found
      {
         while (*str && (*++str != '"'))
            ; // find second quote
      }
      str++;
   }
   return NULL;
}

/// sstrchr with quoted string exception
char *qstrstr(char *str, char *srch)
{
   while (*str)
   {
      for (uint16_t i = 0; srch[i]; i++)
         if (*str == srch[i])
            return str;
      if (*str == '"') // quoted expression found
      {
         while (*str && (*++str != '"'))
            ; // find second quote
      }
      str++;
   }
   return NULL;
}

_cl_param_t *par_param(char *str)
{
   cliParam.argc = 0;
   uint8_t actBuf = pipeBuff.active;
   if (*pipeBuff.str[actBuf])
      pipeBuff.active = pipeBuff.active ? 0 : 1; // change active buffer for the output
#if PIPED_PARAM_AT_END
   cliParam.argv[cliParam.argc] = qstrtok(str, " ,\n\r");
   if (cliParam.argv[cliParam.argc])
      while (++cliParam.argc < MAX_PARAM_ARGUMENTS)
      {
         cliParam.argv[cliParam.argc] = qstrtok(NULL, " ,\n\r");
         if (cliParam.argv[cliParam.argc] == NULL)
            break; // continue to piped params
      }
   if ((!*pipeBuff.str[actBuf]) || (cliParam.argc >= MAX_PARAM_ARGUMENTS))
      return &cliParam;
   cliParam.argv[cliParam.argc] = qstrtok(pipeBuff.str[actBuf], " ,\n\r");
   if (cliParam.argv[cliParam.argc] == NULL)
      return &cliParam;
   while (++cliParam.argc < MAX_PARAM_ARGUMENTS)
   {
      cliParam.argv[cliParam.argc] = qstrtok(NULL, " ,\n\r");
      if (cliParam.argv[cliParam.argc] == NULL)
         return &cliParam;
   }
#else
   if (*pipeBuff.str[actBuf]) // insert piped output as parameters
   {
      cliParam.argv[cliParam.argc] = qstrtok(pipeBuff.str[actBuf], " ,\n\r");
      while (++cliParam.argc < MAX_PARAM_ARGUMENTS - 2)
      {
         cliParam.argv[cliParam.argc] = qstrtok(NULL, " ,\n\r");
         if (cliParam.argv[cliParam.argc] == NULL)
            break;
      }
   }
   cliParam.argv[cliParam.argc] = qstrtok(str, " ,\n\r");
   if (cliParam.argv[cliParam.argc] == NULL)
      return &cliParam;
   while (++cliParam.argc < MAX_PARAM_ARGUMENTS)
   {
      cliParam.argv[cliParam.argc] = qstrtok(NULL, " ,\n\r");
      if (cliParam.argv[cliParam.argc] == NULL)
         return &cliParam;
   }
#endif
   return &cliParam;
}

char *find_char(char *str, char end, char cc)
{
   while (*str && (*str != end))
   {
      if (*str == cc)
         return str;
      str++;
   }
   return NULL;
}

void _pbuff_putc(char c)
{
   pipeBuff.str[pipeBuff.active][pipeBuff.ptr] = c;
   if (pipeBuff.ptr < EL_LENGTH - 1)
      pipeBuff.ptr++;
   pipeBuff.str[pipeBuff.active][pipeBuff.ptr] = '\0';
}
/** Command line execute with pipe support */
uint8_t exec_line(char *str)
{
   enum
   {
      DONE,
      PIPED,
      NEXT
   } state;
   char *token = NULL;
   char *next = NULL;
   char *error;
   uint16_t funcIndx = 0;
   void *lastPutch = stdio->putch;
   *pipeBuff.str[pipeBuff.active] = '\0';
   do
   {
      if (next)
         str = next; // process following command
      while (*str == ' ')
         str++;             // remove leading spaces
      execInterface = NULL; // ifacePath->iface; // set active interface
      /// check for multiple commands with or without piping
      pipeBuff.ptr = 0;
      if ((next = qstrstr(str, ";>")))
      {
         if (*next == '>')
         { // redirect the output to the next command
            stdio->putch = _pbuff_putc;
            state = PIPED;
         }
         else
         { // have following command
            stdio->putch = lastPutch;
            state = NEXT;
         }
         *next = '\0';
         next++;
      }
      else
      {
         stdio->putch = lastPutch;
         state = DONE;
      }

      if ((token = qstrchr(str, ':'))) // check for inerfaces specifier
      {
         if (find_char(str, ':', ' ') == NULL) // there is no space before colon, so it is an interface name
         {
            token = strtok(str, ":");
            if ((execInterface = get_interface(token)) == NULL)
            {
               tprintf("E: Unknown interface: %s\n", token ? token : "\"\"");
               stdio->putch = lastPutch;
               return false;
            }
            if (execInterface->init != NULL)
               if (!execInterface->init(true))
                  execInterface = NULL; // ifacePath->iface; // init unsuccessfull, so set active interface back
         }
      }

      token = strtok(token ? NULL : str, " \n\r");

      if (execInterface && ((token == NULL) || (!strlen(token)))) /// change the interface: "iface:"
      {
         if (ifacePath->iface != execInterface) // the demanded interface is not the active one
         {
            tstrncpy(sysConf.iface, (char *)execInterface->name, IFACE_NAME_LEN);
            conf_save();
            ifacePath->iface = (_iface_t *)execInterface;
         }
         break;
      }
      if (!token)
         continue;
      while (*token == ' ')
         token++; // remove leading spaces

      _iface_list_t *path = ifacePath;
      if (execInterface) /// execute a command from the specified interface: "iface:cmd"
      {
         for (funcIndx = 0; execInterface->cmdList[funcIndx].name != NULL; funcIndx++) /// Check commands
         {
            if (!strcmp(token, execInterface->cmdList[funcIndx].name))
               break;
         }
         if (execInterface->cmdList[funcIndx].name == NULL)
            execInterface = NULL; /// command not found
      }
      else
         while (!execInterface && path)
         {
            for (funcIndx = 0; path->iface->cmdList[funcIndx].name != NULL; funcIndx++) /// Check interface commands
            {
               if (!strcmp(token, path->iface->cmdList[funcIndx].name))
               {
                  execInterface = path->iface;
                  break;
               }
            }
            path = path->next;
         }

      if (execInterface && execInterface->cmdList[funcIndx].func)
      {
         if ((error = execInterface->cmdList[funcIndx].func(par_param(strtok(NULL, "\n\r")))))
         {
            tprintf("E: %s\n", error);
            return false;
         }
         if (state != PIPED)
            *pipeBuff.str[pipeBuff.active] = '\0'; // disable piping for the next param
      }
      else if (token)
      {
         tprintf("E: ");
         if (execInterface && execInterface->deferr)
            tprintf("%s\n", execInterface->deferr);
         else
            tprintf("Invalid command \"%s\"\n", token);
         stdio->putch = lastPutch;
         return false;
      }
   } while (state != DONE);
   stdio->putch = lastPutch;
   return true;
}

void rshell_task(void *vParam)
{
   static char clBuff[EL_LENGTH]; // command line buffer for looped lines
   static char scLine[EL_LENGTH]; // script line buffer
   char *str;
   lfile_t *pathFile;
   uint8_t i = 0;
   _iface_list_t *list = ifaceList;

   shell_version(NULL);
   add_interface(&ifaceUtil);
   interface_registstration();
   draw_icon_rle(288, 0, _rgb(64, 64, 0), ANSI_pal256[sysConf.textBG], IconRsbc);
   tprintf("-- %d bytes free\n", xPortGetFreeHeapSize());
   tprintf("Init: ");
   while (list)
   {
      if (list->iface->init != NULL)
         tprintf("%c", list->iface->init(false) ? '*' : '!');
      list = (_iface_list_t *)list->next;
      i++;
   }
   tprintf(" [%d]\n", i);
   /// Path manegement
   init_iface_path(get_interface(sysConf.iface));
   if ((pathFile = lf_open(PATH_FILE_NAME, MODE_READ)))
   {
      uint16_t fSize = lf_get_fsize("", pathFile->pos >> 8); // lf_get_fsize(PATH_FILE_NAME,0);
      str = pvPortMalloc(fSize + 1);
      lf_read(pathFile, str, fSize);
      str[fSize] = '\0';
      tprintf("Path: %s\n", str);
      lf_close(pathFile);
      iface_path_add(str);
      vPortFree(str);
   }
   tstrncpy(clBuff, sysConf.startup, sizeof(clBuff));
   str = clBuff;
   if (qstrchr(str, '@'))
      str = NULL; // startup script can't be looped
   if (str)
   {
      taskYIELD();
      if (keyboard_break())
         str = NULL; // don't run startup command if CS+Break is pressed
   }
   while (1)
   {
      if (script)
      {
         if (f_gets(scLine, sizeof(scLine), &script->file))
         {
            str = scLine;
         }
         else
         {
            f_close(&script->file);
            vPortEnterCritical();
            vPortFree(script->name);
            vPortFree(script);
            script = script->prev;
            vPortExitCritical();
            str = NULL;
            continue;
         }
      }
      if (!str || !*str)
         str = get_command_line(
             ifacePath->iface->prompt ? ifacePath->iface->prompt : ifacePath->iface->name,
             stdio); // Read str from the command line
      while (*str == ' ')
         str++;        // remove leading spaces
      if (*str == '@') // looped command line
         while (!keyboard_break())
         {
            strcpy(clBuff, str + 1);
            // tprintf("\r");
            if (!exec_line(clBuff))
               break;
         }
      else if (*str != '#')
         exec_line(str); // skip commented lines

      str = NULL; // clear
   }
}

/// ----------------    Shell built-in commands

cmd_err_t shell_help(_cl_param_t *sParam)
{
   const _iface_t *helpIface;
   if (sParam->argc)
   {
      if ((helpIface = get_interface(sParam->argv[0])) != NULL)
         _print_help(helpIface);
      else
         _print_help(&ifaceShell);
   }
   else
      _print_help(ifacePath->iface);
   return CMD_NO_ERR;
}

cmd_err_t shell_echo(_cl_param_t *sParam)
{
   for (uint8_t i = 0; i < sParam->argc; i++)
   {
      tprintf("%s ", sParam->argv[i]);
   }
   tprintf("\n");
   return CMD_NO_ERR;
}

cmd_err_t shell_path(_cl_param_t *sParam)
{
   lfile_t *pathFile;
   _iface_list_t *path = (_iface_list_t *)ifacePath->next; // set to '/'
   bool saveFile = false;
   if (sParam->argc)
   {
      switch (*sParam->argv[0])
      {
      case 'a':
         if (sParam->argc > 1)
         {
            for (uint8_t i = 1; i < sParam->argc; i++)
            {
               if (iface_path_add(sParam->argv[i]))
                  saveFile = true;
            }
         }
         else
            tprintf("Missing ifaces list!");
         break;
      case 'd':
         if (sParam->argc > 1 && *sParam->argv[1] != '*')
         {
            for (uint8_t i = 1; i < sParam->argc; i++)
            {
               if (iface_path_del(sParam->argv[i]))
                  saveFile = true;
            }
            break;
         }
         tprintf("Clear path(y/n)?");
         if (keyboard_wait("yY"))
         {
            iface_path_del("");
            lf_delete(PATH_FILE_NAME);
            tprintf("\nCleared.");
         }
         tprintf("\n");
         break;
      default:
         return "Invalid parameters:\n a - to add\n d - to delete";
      }
      if (saveFile)
      {
         if ((pathFile = lf_open(PATH_FILE_NAME, MODE_CREATE | MODE_WRITE)))
         {
            char *pathListBuff;
            uint8_t pLen = 0;
            if ((pathListBuff = pvPortMalloc(256)) == NULL)
            {
               lf_close(pathFile);
               return "Out of memory";
            }
            *pathListBuff = 0;
            while (path->next)
            {
               path = path->next;
               strncat(pathListBuff, path->iface->name, 255);
               if (path->next)
                  strncat(pathListBuff, ",", 255);
            }
            pLen = strlen(pathListBuff);
            if (pLen)
               lf_write(pathFile, (uint8_t *)pathListBuff, pLen);
            tprintf("Saved: %s\n", pathListBuff);
            lf_close(pathFile);
            vPortFree(pathListBuff);
         }
         tprintf("Path updated.\n");
      }
   }
   else
   {
      path = ifacePath;
      while (1)
      {
         tprintf("%s\n", path->iface->name);
         if (path->next)
            path = path->next;
         else
            break;
      }
   }
   return CMD_NO_ERR;
}

cmd_err_t shell_version(_cl_param_t *sParam)
{
   tprintf("Rimer shell v%d.%2d\n", VERSION, SUBVERSION);
   tprintf("-- Compiled: %s %s\n", __DATE__, __TIME__);
   return CMD_NO_ERR;
}

cmd_err_t shell_ifaces_list(_cl_param_t *sParam)
{
   _iface_list_t *list = ifaceList;
   while (1)
   {
      tprintf(" %s : %s\n", list->iface->name, list->iface->desc);
      if (list->next)
         list = (_iface_list_t *)list->next;
      else
         return CMD_NO_ERR;
   }
}

cmd_err_t shell_cls(_cl_param_t *sParam)
{
   tprintf("\e[2J");
   return CMD_NO_ERR;
}

static cmd_err_t shell_rs(_cl_param_t *sParam) // open script file on the SD card
{
   FIL tmpFile;
   _script_t *tmpScript = script;
   if (!sParam->argc)
      return "File name missing!";
   while (tmpScript) // check for recursion
   {
      if (!(strcmp(tmpScript->name, sParam->argv[0])))
         return "Recursive script detected!";
      tmpScript = tmpScript->prev;
   }
   if (f_open(&tmpFile, sParam->argv[0], FA_READ) != FR_OK)
      return "Can't open script!";
   tmpScript = script;
   if ((script = pvPortMalloc(sizeof(_script_t))))
   {
      if ((script->name = pvPortMalloc(strlen(sParam->argv[0]) + 1)))
         strcpy(script->name, sParam->argv[0]);
      memcpy(&script->file, &tmpFile, sizeof(FIL));
      script->prev = tmpScript;
   }
   return CMD_NO_ERR;
}

static cmd_err_t shell_red(_cl_param_t *sParam) // open script file on the SD card
{
   bool red(char *fileName);
   red(sParam->argc ? sParam->argv[0] : "");
   return CMD_NO_ERR;
}

#if RSHELL_DEBUG
static cmd_err_t shell_param(_cl_param_t *sParam)
{
   tprintf("Param count: %d\n", sParam->argc);
   for (uint8_t i = 0; i < sParam->argc; i++)
      tprintf("[%d]: %s\n", i, sParam->argv[i]);
   return CMD_NO_ERR;
}

static cmd_err_t shell_enum(_cl_param_t *sParam)
{
   static const char *list[] = {
       "one", "two", "three", "four", "five", NULL //
   };
   if (!sParam->argc)
   {
       char **ptr = (char **)list;
       tprintf("Choose one these:\n");
       while (*ptr)
           tprintf("%s, ",*ptr++);
       tprintf("\n");
    return CMD_NO_ERR;
   }
   
   tprintf("Index %d\n", tget_enum(sParam->argv[0],list));
   return CMD_NO_ERR;
}

#include "llfs.h"
#include "llfs_vol_eeprom.h"

static cmd_err_t shell_eefrmt(_cl_param_t *sParam) // format eeprom with llfs
{
   tprintf("Confirm EEPROM formatting (y/n)?");
   if (keyboard_wait("Yy"))
   {
      tprintf("\nFormatting...");
      if ((lf_format(LLFS_EEPROM_SIZE, LLFS_DEVID_EEPROM, "EEPROM") == LF_ERR_NONE))
         tprintf("\nDone!\n");
      else
         return "\nEEPROM format failed!";
   }
   else
      tprintf("\nCancelled.\n");
   return CMD_NO_ERR;
}
#endif