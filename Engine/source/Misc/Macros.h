#pragma once

#define	SAFE_RELEASE(x)	if(x){ x->release(); x = NULL;	}
