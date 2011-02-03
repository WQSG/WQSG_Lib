#ifdef WQSG_LIB_TEST

#include "../WQSG_Lib.h"
#include "../../Common/wqsg_ips.h"
//#include "../../Common/cwqsg_str.h"
int main()
{
	CWQSG_strA strA;
	strA = "123";
	strA.AppendFormat( "%d", 1 );

	CWQSG_strW strW;
	strW = L"123";
	strW.Format( L"%d", 1 );

	CWQSG_File fp;
	fp.OpenFile( L"234" , 1 , 1 );

}

#endif //WQSG_LIB_TEST
