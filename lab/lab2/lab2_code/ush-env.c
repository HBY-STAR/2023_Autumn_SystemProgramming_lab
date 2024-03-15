/* ush-env.c */
/* environment functions for ush */

#include "ush.h"
#include "ush-env.h"
#include "ush-prt.h"

// this data structure is used to store the
// environment assignment information.
struct varslot /* symbol table slot */
{
    char *name;
    char *val;
    BOOLEAN exported;
} sym[MAXVAR];

BOOLEAN assign(char **p, char *s)
{ /* initialize name or value */
    int size;

    size = strlen(s) + 1;
    if (*p == NULL)
    {
        if ((*p = malloc(size)) == NULL)
            return (FALSE);
    }
    else if ((*p = realloc(*p, size)) == NULL)
        return (FALSE);
    strcpy(*p, s);
    return (TRUE);
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment export								       //
//////////////////////////////////////////////////////////
BOOLEAN EVexport(char *name)
{ /* set variable to be exported */
    struct varslot *v;
    v = find(name);
    if (v == NULL || v->name == NULL)
    {
        return (FALSE);
    }
    v->exported = TRUE;
    return (TRUE);
}

struct varslot *find(char *name)
{ /* find symbol table entry */

    int i;
    struct varslot *v;

    v = NULL;
    for (i = 0; i < MAXVAR; i++)
        if (sym[i].name == NULL)
        {
            if (v == NULL)
                v = &sym[i];
        }
        else if (strcmp(sym[i].name, name) == 0)
        {
            v = &sym[i];
            break;
        }
    return (v);
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment set								       //
//////////////////////////////////////////////////////////

BOOLEAN EVset(char *name, char *val)
{ /* add name & valude to enviromnemt */
    struct varslot *v;
    v = find(name);
    if (v == NULL)
    {
        return (FALSE);
    }
    if (v->name == NULL)
    {
        v->name = calloc(200, 1);
        if (v->name == NULL)
        {
            return (FALSE);
        }
        strcat(v->name, name);
    }

    if (v->val != NULL)
    {
        free(v->val);
    }
    v->val = calloc(200, 1);
    if (v->val == NULL)
    {
        return (FALSE);
    }
    strcat(v->val, val);
    v->exported = FALSE;
    return (TRUE);
}

BOOLEAN EVinit()
{ /* initialize symbol table from
     environment */
    int i, namelen;
    char name[100];

    // Get home and path
    char *temp;
    temp = getenv("HOME");
    environ[0] = calloc(200, 1);
    strcat(environ[0], "HOME=");
    strcat(environ[0], temp);
    // printf("%s\n", environ[0]);

    temp = getenv("PATH");
    environ[1] = calloc(200, 1);
    strcat(environ[1], "PATH=");
    strcat(environ[1], temp);
    // char *wd = getcwd(NULL, 0);
    // if (NULL == wd)
    // {
    //     return (FALSE);
    // }
    // strcat(environ[1], ":");
    // strcat(environ[1], wd);
    // printf("%s\n", environ[1]);

    temp = getenv("PWD");
    environ[2] = calloc(200, 1);
    strcat(environ[2], "PWD=");
    strcat(environ[2], temp);
    // printf("%s\n", environ[2]);

    environ[3] = NULL;

    for (i = 0; environ[i] != NULL; i++)
    {
        namelen = strcspn(environ[i], "=");
        strncpy(name, environ[i], namelen);
        name[namelen] = '\0';
        if (!EVset(name, &environ[i][namelen + 1]) || !EVexport(name))
            return (FALSE);
    }
    free(environ[0]);
    free(environ[1]);
    free(environ[2]);
    return (TRUE);
}

char *EVget(char *name)
{ /* get value of variable */
    struct varslot *v;

    if ((v = find(name)) == NULL || v->name == NULL)
        return (NULL);
    return (v->val);
}

BOOLEAN EVunset(char *name)
{ /* add name & valude to enviromnemt */
    struct varslot *v;
    v = find(name);
    if (v == NULL || v->name == NULL)
    {
        return (FALSE);
    }
    free(v->name);
    if (v->val != NULL)
    {
        free(v->val);
    }
    v->name = NULL;
    v->val = NULL;
    v->exported = FALSE;
    return (TRUE);
}

void EVprint()
{ /* printf environment */
    int i;

    for (i = 0; i < MAXVAR; i++)
        if (sym[i].name != NULL)
            printf("%3s %s=%s\n", sym[i].exported ? "[E]" : "",
                   sym[i].name, sym[i].val);
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment assignment						       //
//////////////////////////////////////////////////////////
void unset(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (!EVunset(argv[i]))
        {
            syserr("unset error");
        }
    }
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment assignment						       //
//////////////////////////////////////////////////////////
void set(int argc, char *argv[])
{ /* set command */
    if (argc == 1)
    {
        EVprint();
        return;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            int namelen = strcspn(argv[i], "=");
            if (namelen == strlen(argv[i]))
            {
                syserr("set error");
            }
            else
            {
                char name[100] = {'\0'};
                strncpy(name, argv[i], namelen);
                name[namelen] = '\0';
                if (!EVset(name, &argv[i][namelen + 1]))
                {
                    syserr("set error");
                }
            }
        }
    }
}

void export(int argc, char *argv[])
{ /* export command */
    int i;

    if (argc == 1)
    {
        set(argc, argv);
        return;
    }
    for (int i = 1; i < argc; i++)
    {
        int namelen = strcspn(argv[i], "=");
        if (namelen == strlen(argv[i]))
        {
            if (!EVexport(argv[i]))
            {
                printf("Cannot export %s\n", argv[i]);
                return;
            }
        }
        else
        {
            char name[100];
            strncpy(name, argv[i], namelen);
            name[namelen] = '\0';
            if (!EVset(name, &argv[i][namelen + 1]) || !EVexport(name))
            {
                syserr("export error");
            }
        }
    }
}

void set_shell_env()
{
    for (int i = 0; i < MAXVAR; i++)
    {
        if (sym[i].name != NULL)
        {
            if (sym[i].exported)
            {
                char *val = calloc(200, 1);
                strcat(val, sym[i].val);
                // if (sym[i].name == "PATH")
                if (strcmp(sym[i].name, "PATH") == 0)
                {
                    struct varslot *v = find("PWD");
                    if (v->name != NULL && v->val != NULL)
                    {
                        strcat(val, ":");
                        strcat(val, v->val);
                    }
                }
                setenv(sym[i].name, val, 1);
                free(val);
            }
        }
    }
}
