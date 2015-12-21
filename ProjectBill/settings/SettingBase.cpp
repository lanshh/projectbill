#include "../stdafx.h"
#include "SettingBase.h"
#include "../common/utils.h"
const TCHAR * base64char = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
TCHAR * base64_encode( const TCHAR * bindata, TCHAR * base64, int binlength )
{
    int         i;
    int         j;
    TCHAR       current;
    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (TCHAR)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (TCHAR)(bindata[i] << 4 ) ) & ( (TCHAR)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = TEXT('=');
            base64[j++] = TEXT('=');
            break;
        }
        current |= ( (TCHAR)(bindata[i+1] >> 4) ) & ( (TCHAR) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (TCHAR)(bindata[i+1] << 2) ) & ( (TCHAR)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = TEXT('=');
            break;
        }
        current |= ( (TCHAR)(bindata[i+2] >> 6) ) & ( (TCHAR) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (TCHAR)bindata[i+2] ) & ( (TCHAR)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = TEXT('\0');
    return base64;
}

int base64_decode( const TCHAR * base64, TCHAR * bindata )
{
    int     i;
    int     j;
    TCHAR   k;
    TCHAR   temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((TCHAR)(((TCHAR)(temp[0] << 2))&0xFC))|((TCHAR)((TCHAR)(temp[1]>>4)&0x03));
        if ( base64[i+2] == TEXT('=') )
            break;

        bindata[j++] = ((TCHAR)(((TCHAR)(temp[1] << 4))&0xF0))|((TCHAR)((TCHAR)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == TEXT('='))
            break;
        bindata[j++] = ((TCHAR)(((TCHAR)(temp[2] << 6))&0xF0)) |((TCHAR)(temp[3]&0x3F));
    }
    return j;
}

CIniSettingBase::~CIniSettingBase()
{
    if(pIniFile) {
        delete pIniFile;
        /* pIniFile = NULL; **/
    }
}
std::wstring CIniSettingBase::EncryptPassWord(std::wstring plaintext)
{
    TCHAR ciphertext[128] = {0};
    base64_encode( plaintext.c_str(), ciphertext,plaintext.length() );
    return ciphertext;
}
std::wstring CIniSettingBase::DecryptPassWord(std::wstring ciphertext)
{
    TCHAR plaintext[128] = {0};
    base64_decode( ciphertext.c_str(),plaintext);
    return plaintext;
}

bool CIniSettingBase::LoadToolSetting(std::wstring strConfig)
{
    szFileName  = strConfig;
    pIniFile    = new CIniFile;
    if(!pIniFile) return false;
    bool bRet = pIniFile->Load(szFileName, false);
    if (!bRet) {
        delete pIniFile;
        pIniFile = NULL;
        return false;
    }
    szLan               = GetStr(TEXT("CONFIG:Lan"));
    nLan                = _wtoi(GetStr(TEXT("NLAN")).c_str());
    bDebug              = 1 == _wtoi(GetStr(TEXT("Debug")).c_str());
    /*recovery **/
    bRecovery           = 1 == _wtoi(GetStr(TEXT("RECOVERY")).c_str());
    strRecoveryFlag     = GetStr(TEXT("RECOVERYFLAG")).c_str();
    strRecoveryTimeFlag = GetStr(TEXT("RECOVERYTIMEFLAG")).c_str();

    bVersionCheck       = 1 == _wtoi(GetStr(TEXT("VERSIONCHECK")).c_str());
    strDeviceVersion    = GetStr(TEXT("DEVICEVERSION"));
    bReadInfo           =1 == _wtoi(GetStr(TEXT("READINFO")).c_str());

    bCheckDevicePower   = 1 == _wtoi(GetStr(TEXT("CHECKDEVICEPOWER")).c_str());
    nPowerLimitLow      = _wtoi(GetStr(TEXT("POWERLIMITLOW")).c_str());
    nPowerLimitHigh     = _wtoi(GetStr(TEXT("POWERLIMITHIGH")).c_str());

    bReadDdrInfo        = bReadFlashInfo = 1 == _wtoi(GetStr(TEXT("MEMORYINFO")).c_str());
    strFlashSize        = GetStr(TEXT("FLASHSIZE"));
    strDdrSize          = GetStr(TEXT("DDRSIZE"));
    

    bReboot             = 1 == _wtoi(GetStr(TEXT("REBOOT")).c_str());
    nNextMode           = _wtoi(GetStr(TEXT("NEXTMODE")).c_str());
    bUseAtCom           = 1 == _wtoi(GetStr(TEXT("USEATCOM")).c_str());
    bConnectDataBase    = 1 == _wtoi(GetStr(TEXT("PULLFROMSQL")).c_str());
    nPrimaryKey         = _wtoi(GetStr(TEXT("PRIMARYKEY")).c_str());

    int nItemNum;
    nItemNum = FLAG_CNT;
    ParseStr(GetStr(TEXT("ITEMNAME")),strItemName,&nItemNum);
    if(FLAG_CNT != nItemNum) {
        return false;
    }
    nItemNum = FLAG_CNT;
    ParseStr(GetStr(TEXT("ITEMFLAG")),strItemFlag,&nItemNum);
    if(FLAG_CNT != nItemNum) {
        return false;
    }

    nItemNum = FLAG_CNT;
    ParseBools(GetStr(TEXT("ITEMUSED")),bItemUsed,&nItemNum);
    if(FLAG_CNT != nItemNum) {
        return false;
    }

    /*Main UI**/
    nItemNum = FLAG_CNT;
    ParseStr(GetStr(TEXT("ITEMSTART")),strItemStart,&nItemNum);
    /*if(FLAG_CNT != nItemNum) {
        return false;
    } **/
    
    nItemNum = FLAG_CNT;
    ParseIntegers(GetStr(TEXT("ITEMCOUNT")),nItemCount,&nItemNum);
    /*if(FLAG_CNT != nItemNum) {
        return false;
    }**/

    nItemNum = FLAG_CNT;
    ParseIntegers(GetStr(TEXT("ITEMSPAN")),nItemSpan,&nItemNum);
    /*if(FLAG_CNT != nItemNum) {
        return false;
    }**/

    nColCount = _wtoi(GetStr(TEXT("TBCOLCOUNT")).c_str());
    nItemNum = MAX_COLUMN;
    ParseStr(GetStr(TEXT("TBCOLNAME")),strTableColumnName,&nItemNum);
    if(nColCount != nItemNum) return false;

    nItemNum = MAX_COLUMN;
    ParseStr(GetStr(TEXT("TBCOLTYPE")),strTableType,&nItemNum);
    if(nColCount != nItemNum) return false;

    nItemNum = MAX_COLUMN;
    ParseBools(GetStr(TEXT("TBCOLNULL")),nCanBeNull,&nItemNum);
    if(nColCount != nItemNum) return false;



    
    nSupportDatabse = 6;
    ParseStr(GetStr(TEXT("SUPPORTDB")),strSupportDatabse,&nSupportDatabse);
    nDataBaseType   =_wtoi(GetStr(TEXT("DETABASETYPE")).c_str());

    strServer       =GetStr(TEXT("DETABASESERVER"));
    strUserName     =GetStr(TEXT("USERNAME"));
    strPassword     =DecryptPassWord(GetStr(TEXT("PASSWORD")));          /*do not save directory **/
    strDataBaseTable=GetStr(TEXT("DBTABLE"));
    strDataBaseName =GetStr(TEXT("DBNAME"));
    strPort         =GetStr(TEXT("DATABASEPORT")); 

    strWriteFlag    =GetStr(TEXT("WRITEFLAG"));
    strReadFlag     =GetStr(TEXT("READFLAG")); 
    strReadTimeFlag =GetStr(TEXT("READTIMEFLAG"));
    strWriteTimeFlag=GetStr(TEXT("WRITETIMEFLAG"));

    
    return true;
}
bool CIniSettingBase::SaveToolSetting(std::wstring strConfig)
{

    std::wstring checke = TEXT("1"),unckeck = TEXT("0");
    std::wstring szValue1,szValue2;
    TCHAR szTemp1[256],szTemp2[64];
    if(pIniFile) {
        swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nLan);
        SetStr( TEXT("CONFIG:NLAN")     , std::wstring(szTemp1));
        /*recovery **/
        SetStr( TEXT("RECOVERY")        , bRecovery?checke:unckeck);
        SetStr( TEXT("RECOVERYFLAG")    , strRecoveryFlag);
        SetStr( TEXT("RECOVERYTIMEFLAG"), strRecoveryTimeFlag);
        
        SetStr( TEXT("VERSIONCHECK")    , bVersionCheck?checke:unckeck);
        SetStr( TEXT("DEVICEVERSION")   , strDeviceVersion);
        SetStr( TEXT("READINFO")        , bReadInfo?checke:unckeck);
        SetStr( TEXT("CHECKDEVICEPOWER"), bCheckDevicePower?checke:unckeck);
        swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nPowerLimitLow);
        SetStr( TEXT("POWERLIMITLOW")   , std::wstring(szTemp1));
        swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nPowerLimitHigh);
        SetStr( TEXT("POWERLIMITHIGH")  , std::wstring(szTemp1));

        SetStr( TEXT("REBOOT")          , bReboot?checke:unckeck);
        swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nNextMode);
        SetStr( TEXT("NEXTMODE")        , std::wstring(szTemp1));

        /*Memoery check **/
        SetStr( TEXT("MEMORYINFO")      , bReadFlashInfo?checke:unckeck);
        SetStr( TEXT("FLASHSIZE")       , strFlashSize);
        SetStr( TEXT("DDRSIZE")         , strDdrSize);
        
        SetStr( TEXT("USEATCOM")        , bUseAtCom?checke:unckeck);
        SetStr( TEXT("PULLFROMSQL")     , bConnectDataBase?checke:unckeck);

        swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nDataBaseType);
        SetStr( TEXT("DETABASETYPE")    , std::wstring(szTemp1));
        SetStr( TEXT("DETABASESERVER")  , strServer);
        SetStr( TEXT("USERNAME")        , strUserName);
        SetStr( TEXT("PASSWORD")        , EncryptPassWord(strPassword));        /*do not save directory **/
        SetStr( TEXT("DBTABLE")         , strDataBaseTable);
        SetStr( TEXT("DBNAME")          , strDataBaseName);
        SetStr( TEXT("DATABASEPORT")    , strPort); 
        
        swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nPrimaryKey);
        SetStr( TEXT("PRIMARYKEY")      , std::wstring(szTemp1)); 

        SetStr( TEXT("WRITEFLAG")       , strWriteFlag); 
        SetStr( TEXT("READFLAG")        , strReadFlag); 
        SetStr( TEXT("WRITETIMEFLAG")   , strWriteTimeFlag); 
        SetStr( TEXT("READTIMEFLAG")    , strReadTimeFlag); 
        /*database item ***/
        std::wstring    item;
        int             i;
        /*item table name **/
        for( i = 0; i < FLAG_CNT; i ++) {
            if(0==i) {
                item = strItemName[i];
            } else {
                item += (TEXT(",") + strItemName[i]);
            }

        }
        SetStr( TEXT("ITEMNAME"),item); 

        /*item table flag for futear usage **/
        for( i = 0; i < FLAG_CNT; i ++) {
            if(0==i) {
                item = strItemFlag[i];
            } else {
                item += (TEXT(",") + strItemFlag[i]);
            }
        
        }
        SetStr( TEXT("ITEMFLAG"),item); 

        /*ITEMUSED **/
        for( i = 0; i < FLAG_CNT; i ++) {
            if(0==i) { 
                item = (bItemUsed[i]?checke:unckeck);
            } else {
                item += (TEXT(",") + (bItemUsed[i]?checke:unckeck));
            }
        
        }
        SetStr( TEXT("ITEMUSED"),item);

        for( i = 0; i < FLAG_CNT; i ++) {
            if(0==i) {
                item = strItemStart[i];
            } else {
                item += (TEXT(",") + strItemStart[i]);
            }
        }
        SetStr( TEXT("ITEMSTART"),item); 

        for( i = 0; i < FLAG_CNT; i ++) {
            swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nItemCount[i]);
            if(0==i) {
                item = std::wstring(szTemp1);
            } else {
                item += (TEXT(",") + std::wstring(szTemp1));
            }
        }
        SetStr( TEXT("ITEMCOUNT"),item); 

        for( i = 0; i < FLAG_CNT; i ++) {
            swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nItemSpan[i]);
            if(0==i) {
                item = std::wstring(szTemp1);
            } else {
                item += (TEXT(",") + std::wstring(szTemp1));
            }
        }
        SetStr( TEXT("ITEMSPAN"),item); 

        /*Create Table Parms**/

        if(nColCount > 100 ) nColCount = 100;
        swprintf(szTemp1,sizeof(szTemp1)/sizeof(szTemp1[0]),TEXT("%d"),nColCount);
        SetStr( TEXT("TBCOLCOUNT")      , std::wstring(szTemp1)); 

        for( i = 0; i < nColCount; i ++) {
            if(0==i) {
                item = strTableColumnName[i];
            } else {
                item += (TEXT(",") + strTableColumnName[i]);
            }
        }
        SetStr( TEXT("TBCOLNAME"),item); 

        for( i = 0; i < nColCount; i ++) {
              if(0==i) {
                  item = strTableType[i];
              } else {
                  item += (TEXT(",") + strTableType[i]);
              }
          }
        SetStr( TEXT("TBCOLTYPE"),item); 


        for( i = 0; i < nColCount; i ++) {
            if(0==i) { 
                item = (nCanBeNull[i]?checke:unckeck);
            } else {
                item += (TEXT(",") + (nCanBeNull[i]?checke:unckeck));
            }
        
        }
        SetStr( TEXT("TBCOLNULL"),item);


        return pIniFile->Save(szFileName);
    }
    return false;
}
std::wstring CIniSettingBase::GetStr(std::wstring key)
{
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return TEXT("");
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pCurSection = pIniFile->GetSection(szSection);
        szKey = key.substr(npos +1);
    } else {
        if(NULL == pCurSection) {
            return TEXT("");
        }
        szKey = key;
    }
    if(key.empty()) return TEXT("");
    if(pCurSection) return pCurSection->GetKeyValue(szKey);
    return TEXT("");
}
bool CIniSettingBase::SetStr(std::wstring key,std::wstring &value)
{
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return false;
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pCurSection = pIniFile->AddSection(szSection);
        szKey = key.substr(npos +1);
    } else {
        if(NULL == pCurSection) {
            return false;
        }
        szKey = key;
    }
    if(key.empty())         return false;
    if(NULL == pCurSection) return false;
    pCurSection->SetKeyValue(szKey,value);
    return true;
}

bool CIniLocalLan::LoadToolSetting(std::wstring strConfig)
{
    szFileName  = strConfig;
    pIniFile    = new CIniFile;
    if(!pIniFile) return false;
    bool bRet = pIniFile->Load(szFileName, false);
    if (!bRet) {
        delete pIniFile;
        pIniFile = NULL;
        return false;
    }
    /*pCurSection = pIniFile->GetSection(TEXT("LANG")); **/
    return true;
}
bool CIniLocalLan::SaveToolSetting(std::wstring strConfig)
{
    if(!strConfig.empty()) szFileName = strConfig;
    if(pIniFile)
        return pIniFile->Save(szFileName);
    return false;

}
bool CIniLocalLan::SetStr(std::wstring key,std::wstring &value)
{
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return false;
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pCurSection = pIniFile->AddSection(szSection);
        szKey = key.substr(npos +1);
    } else {
        if(NULL == pCurSection) {
            return false;
        }
        szKey = key;
    }
    if(key.empty())         return false;
    if(NULL == pCurSection) return false;
    pCurSection->SetKeyValue(szKey,value);
    return true;
}
std::wstring CIniLocalLan::GetStr(std::wstring key,std::wstring default) 
{
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return default;
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pCurSection = pIniFile->GetSection(szSection);
        szKey = key.substr(npos +1);
    } else {
        if(NULL == pCurSection) {
            return default;
        }
        szKey = key;
    }
    if(key.empty()) return default;
    if(pCurSection) return pCurSection->GetKeyValue(szKey);
    return default;

}
std::wstring CIniLocalLan::GetStr(std::wstring key) 
{
    std::wstring szSection;
    std::wstring szKey;
    if(NULL == pIniFile) return TEXT("");
    std::wstring::size_type npos = key.find_first_of(TEXT(":"));
    if(npos != std::wstring::npos) {
        szSection = key.substr(0,npos);
        pCurSection = pIniFile->GetSection(szSection);
        szKey = key.substr(npos +1);
    } else {
        if(NULL == pCurSection) {
            return TEXT("");
        }
        szKey = key;
    }
    if(key.empty()) return TEXT("");
    if(pCurSection) return pCurSection->GetKeyValue(szKey);
    return TEXT("");

}
void CIniSettingBase::ParseBools(std::wstring s, bool *pBool, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for( j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pBool[j++] = 1 == _wtoi(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pBool[j++] = 1 == _wtoi(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pBool[j++] = false;
        }
    }
    *pLen = j;
    return;
}
void CIniSettingBase::ParseIntegers(std::wstring s, double *pInt, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for( j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = _wtof(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pInt[j++] = _wtof(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pInt[j++] = 0;
        }
    }
    *pLen = j;
    return;
}
void CIniSettingBase::ParseIntegers(std::wstring s, float *pInt, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for(j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = _wtof(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pInt[j++] = _wtof(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pInt[j++] = 0;
        }
    }
    *pLen = j;
/*parseintegers_exit: **/
    return;
}

void CIniSettingBase::ParseIntegers(std::wstring s, int *pInt, int *pLen)
{
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    posl    = 0;
    for(j = 0;j  < iLen ;) {
        posr = s.find_first_of(TEXT(","),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = _wtoi(subs.c_str());
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl);
            pInt[j++] = _wtoi(subs.c_str());
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            pInt[j++] = 0;
        }
    }
    *pLen = j;
/*parseintegers_exit: **/
    return;
}
void CIniSettingBase::ParseIp(std::wstring s, unsigned int *IPV4)
{

    unsigned char           IP;
    int                     j;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    if (s.empty()) return;
    posl    = 0;
    *IPV4   = 0;
    for(j = 0;j< 4 ;) {
        posr = s.find_first_of(TEXT(" ."),posl);
        if(std::string::npos != posr&&posr > posl) {
            subs    = s.substr(posl,posr - posl );
            posl    = posr + 1;
            IP      = _wtoi(subs.c_str());
            *IPV4   = ((*IPV4)<<8)|IP;
            j++;
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs    = s.substr(posl);
            IP      = _wtoi(subs.c_str());
            *IPV4   = ((*IPV4)<<8)|IP;
            j++;
            break;
        } 
        
    }
    /*parseintegers_exit: **/
    return;


}
void CIniSettingBase::ParseStr(std::wstring s, std::wstring *pInt, int *pLen)
{
    size_t                  StrLen;
    int                     j,iLen    = *pLen;
    std::wstring            subs;
    std::string::size_type  posl,posr;
    *pLen = 0;
    if (s.empty()) return;
    StrLen  = s.length();
    posl    = 0;
    for(j = 0;j  < iLen ;) {
        if(posl > StrLen) return ; /*
                                     except out range,but posl always no greater StrLen
                                     posl = posr + 1;
                                     but posr will never equal to StrLen 
                                   **/
        posr = s.find_first_of(TEXT(","),posl); /*posl string posr **/
        if(std::string::npos != posr&&posr > posl) {
            subs = s.substr(posl,posr - posl );
            posl = posr + 1;
            pInt[j++] = subs.c_str();
        } else if (std::string::npos == posr&&std::string::npos != posl){
            subs = s.substr(posl); /*If this is equal to the string length, 
                                     the function returns an empty string. **/
            pInt[j++] = subs;
            break;
        } else if (std::string::npos != posr&&posr == posl) { /*** ,,***/
            subs.clear();
            posl = posr + 1;
            pInt[j++] = subs.c_str();
        }
    }
    *pLen = j;
/*parseintegers_exit: **/
    return;
}


static BOOL CALLBACK SetStringProc(HWND hwnd,LPARAM lParam )
{
    return ((CIniLocalLan*)lParam)->SetStringProc((void *)hwnd,(void *)lParam);
}
static BOOL CALLBACK GetStringProc(HWND hwnd,LPARAM lParam )
{
    return ((CIniLocalLan*)lParam)->GetStringProc((void *)hwnd,(void *)lParam);
}
void CIniLocalLan::TreeMenu(void * pParam,std::wstring strMainKeyPart)
{
    HMENU   pMenu = (HMENU)pParam;
    TCHAR   strKey[260];
    if(NULL == pMenu) return ;
    for (int i=0;i < GetMenuItemCount(pMenu);i++) {
        swprintf(strKey,nof(strKey),TEXT("%s_%d"),strMainKeyPart.c_str(),i);
        int id = GetMenuItemID(pMenu,i);
        if (0 == id) { 
            /*If nPos corresponds to a SEPARATOR menu item, the return value is 0. **/
        } else if (-1 == id) { 
            /*If the specified item is a pop-up menu (as opposed to an item within the pop-up menu), the return value is ¨C1 **/
            ModifyMenu(pMenu,i, MF_BYPOSITION, i, GetStr(strKey).c_str());
        	TreeMenu(GetSubMenu(pMenu,i),strKey);
        } else {
            ModifyMenu(pMenu,id, MF_BYCOMMAND, id, GetStr(strKey).c_str());
        }
    }
}
bool CIniLocalLan::TreeControls(void * pParam,BOOL bSvae,int DlgId,bool bVer)
{
    HWND            hWnd =  (HWND)pParam;
    BOOL            bReturn;
    TCHAR           strSection[260] = {0},strValue[260] = {0};
    long            resID = DlgId;
    swprintf(strSection,nof(strSection),TEXT("DIALOG_%d:DIALOG_TITLE"),DlgId);
    if(bSvae) {
        GetWindowText(hWnd,strValue,260);
        SetStr((LPCTSTR)strSection,std::wstring(strValue));
        bReturn = EnumChildWindows(hWnd,(WNDENUMPROC)::GetStringProc,(LPARAM)this);
        SaveToolSetting(TEXT(""));
    } else {
        std::wstring  strTitle = GetStr((LPCTSTR)strSection) + (bVer?TEXT(APP_VER):TEXT(""));
        SetWindowText(hWnd,strTitle.c_str());
        bReturn = EnumChildWindows(hWnd,(WNDENUMPROC)::SetStringProc,(LPARAM)this);
    }
    /*APPNAME = GetStr(TEXT("LANG:APPNAME"));**/
    if (!bReturn) {
        return false;
    }
    return true;
}
bool CIniLocalLan::GetStringProc(void * pParam,void* lParam )
{
    HWND    hwnd    = (HWND)pParam;
    int     resID   = GetWindowLong(hwnd,GWL_ID);
    if (resID==0) {
        return TRUE;
    }
    TCHAR strKey[256],strValue[256],szClassName[256] = TEXT("\0");
    if (GetClassName(hwnd,szClassName,256)==0) {
        return false;
    }
    if (_wcsicmp(szClassName,TEXT("Button"))==0) {
        swprintf(strKey,nof(strKey),TEXT("BUTTON_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("Static"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LABEL_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("SysTreeView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TREEVIEW_%d"),resID);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysListView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LISTVIEW_%d"),resID);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysTabControl32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TABCTRL_%d"),resID);
        return true;
    } else {
        return true;
    }
    GetWindowText(hwnd,strValue,nof(strValue));
    SetStr(strKey,std::wstring((LPCTSTR)strValue));
    return true;
}
bool CIniLocalLan::SetStringProc(void * pParam,void* lParam )
{
    HWND hwnd = (HWND)pParam;
    int resID = GetWindowLong(hwnd,GWL_ID);
    if (resID==0) {
        return TRUE;
    }
    TCHAR strKey[256];
    TCHAR szClassName[256] = {0};
    if (GetClassName(hwnd,szClassName,256)==0) {
        return false;
    }
    if (_wcsicmp(szClassName,TEXT("Button"))==0) {
        swprintf(strKey,nof(strKey),TEXT("BUTTON_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("Static"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LABEL_%d"),resID);
    } else if (_wcsicmp(szClassName,TEXT("SysTreeView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TREEVIEW_%d"),resID);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysListView32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("LISTVIEW_%d"),resID);
        return true;
    } else if (_wcsicmp(szClassName,TEXT("SysTabControl32"))==0) {
        swprintf(strKey,nof(strKey),TEXT("TABCTRL_%d"),resID);
        return true;
    } else {
        return true;
    }
    SetWindowText(hwnd,GetStr(strKey).c_str());
    return true;
}

TCHAR GenCD(INT64 nImei)
{
    INT64   d;
    int     nTime       = 0;
    INT64   nCD         = 0;  
    while (nImei != 0) {
        d = nImei % 10;
        if(nTime % 2 == 0) {
            d *= 2;
            if(d >= 10) {
                d = d % 10 + d / 10;
            }
        }   
        nCD     += d;   
        nTime++;
        nImei   /= 10;
    }  
    nCD %= 10;
    if(nCD != 0) {
        nCD = 10 - nCD;
    }  
    return (TCHAR)nCD;
}
bool CheckMac(const TCHAR *szImei,unsigned int  flag)
{
    int         nIndex;
    int         len;
    if(NULL == szImei ) {
        return false;
    }
    len = _tcslen( szImei );
    if ( MAC_LEN*2 == len ) {
        nIndex = _tcsspn( szImei , TEXT("0123456789ABCDEFabcdef"));
        return ( len == nIndex ) ? TRUE : FALSE;
    }
    return false;
}
bool CheckMac(const char  *szImei ,unsigned int  flag)
{
    int         nIndex;
    int         len;
    if(NULL == szImei ) {
        return FALSE;
    }
    len = strlen( szImei );
    if ( MAC_LEN*2 == len ) {
        nIndex = strspn( szImei , "0123456789ABCDEFabcdef");
        return ( len == nIndex ) ? TRUE : FALSE;
    }
    return FALSE;
}
bool CheckItemValid(const TCHAR *szItem,unsigned int  flag)
{
    if(FLAG_SN == flag) {
        DWORD dwStrLen = _tcslen(szItem);
        if(0 == dwStrLen) {
            goto tag_exit;
        }
    } else if (FLAG_WIFIMAC== flag||FLAG_BTMAC== flag) {
        if(!CheckMac(szItem,0)) {
            goto tag_exit;
        }
    } else if (FLAG_IMEI1== flag||FLAG_IMEI2== flag) {
        if(!CheckImei(szItem,0)) {
            goto tag_exit;
        }
    } else {
        return FALSE;
    }
    return TRUE;
tag_exit:
    return FALSE;
}
bool CheckImei(const char *szImei,unsigned int  flag)
{
    __int64         int64Imei;
    size_t          nLen;
    bool            bLen1;
    bool            bLen2;
    if (NULL == szImei) {
        return false;
    }
    if(0 == flag) {
        bLen1 = bLen2 = 1;
    } else {
        bLen1 = 0 != (flag&1);
        bLen2 = 0 != (flag&2);
    }
    nLen    = strlen( szImei );
    if (bLen2&&IMEI_LEN == nLen) {
        if( nLen == strspn( szImei,"0123456789")){
            int64Imei   = _atoi64(szImei);
            int64Imei   /= 10;
            return  szImei[IMEI_LEN -1] == GenCD(int64Imei) + '0';
        }
    } else if (bLen1&&IMEI_LEN -1 == nLen) {
        return nLen == strspn( szImei,"0123456789");
    }
    return false;
}
bool CheckImei(const TCHAR *szImei ,unsigned int  flag)
{
    __int64    int64Imei;
    size_t     nLen;
    bool       bLen1;
    bool       bLen2;
    if (NULL == szImei) {
        return false;
    }
    if(0 == flag) {
        bLen1 = bLen2 = 1;
    } else {
        bLen1 = 0 != (flag&1);
        bLen2 = 0 != (flag&2);
    }
    nLen    = _tcslen( szImei );
    if (bLen2&&(IMEI_LEN == nLen)) {
        if( nLen == _tcsspn( szImei,TEXT("0123456789"))){
            int64Imei   = _ttoi64(szImei);
            int64Imei   /= 10;
            return  szImei[IMEI_LEN -1] == GenCD(int64Imei) + TEXT('0');
        }
    } else if (bLen1&&(IMEI_LEN -1 == nLen)) {
        return nLen == _tcsspn(szImei,TEXT("0123456789"));
    }
    return FALSE;
}
bool CheckImeiAddCD(TCHAR *szImei)
{
    bool        bRet        = false;
    INT         strLen      = 0;
    INT64       int64IMEI   = 0;
    if(!szImei) return false;
    strLen = _tcslen(szImei);
    if (IMEI_LEN == strLen) {
        if( strLen == _tcsspn( szImei,_T("0123456789"))){
            int64IMEI   = _ttoi64(szImei);
            int64IMEI   /= 10;
            bRet        = szImei[IMEI_LEN -1] == GenCD(int64IMEI) + TEXT('0');
        }
    } else if (IMEI_LEN -1 == strLen ) {
        if( strLen == _tcsspn(szImei,_T("0123456789"))){
            int64IMEI           = _ttoi64(szImei);
            szImei[IMEI_LEN -1] = GenCD(int64IMEI) + TEXT('0');
            bRet                = true;
        }
    }
    return bRet;
}
bool ImeiInc(TCHAR *szImei,int span)
{
    INT64   int64Imei   = 0;
    int     ImeiLength = _tcslen(szImei);
    if(IMEI_LEN == ImeiLength) {
        szImei[IMEI_LEN - 1] = 0;
        IntStrIncreaseSkipAlpha(szImei);
        int64Imei = _ttoi64(szImei);
        /*int64Imei   /= 10; **/
        szImei[IMEI_LEN -1] = GenCD(int64Imei) + TEXT('0');
        return TRUE;
    } else if(IMEI_LEN - 1== ImeiLength){
        IntStrIncreaseSkipAlpha(szImei);
        return TRUE;
    }
    return FALSE;
}   


