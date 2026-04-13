#include "tim_manager.h"
#include "string.h"
struct Tim_Dev *g_TimDevs 		= NULL;
struct Tim_Dev *g_TimDevsTail = NULL; 

void RegisterTim(struct Tim_Dev * PTimOpr)
{
    if (g_TimDevs == NULL) 
		{
        g_TimDevs = PTimOpr;
        g_TimDevsTail = PTimOpr;
    } 
		else 
		{
        g_TimDevsTail->ptNext = PTimOpr;
        g_TimDevsTail = PTimOpr;
    }
    PTimOpr->ptNext = NULL; 
}

struct Tim_Dev * SelectDefaultTim(char *name)
{
	struct Tim_Dev * pTmp = g_TimDevs;
	while (pTmp)
	{
		if (strcmp(name, pTmp->name) == 0)
		{
			return pTmp;
		}
		pTmp = pTmp->ptNext;
	}
 
	return NULL;
}


struct Tim_Dev * Tim_Get_Devs(void)
{
	return g_TimDevs;
}



