#ifndef _SETTINGBASE_H_
#define _SETTINGBASE_H_
#define APP_VER " V1.0.1"
#include "./inifile.h"
#include <vector>
#define LAN_ACTION TRUE
#define nof(x) (sizeof(x) / sizeof(x[0]))
/********** FLAG_TYPE ***********/
#define FLAG_SN                 0
#define FLAG_MAC                1
#define FLAG_WIFIMAC            1
#define FLAG_BTMAC              2
#define FLAG_IMEI1              3
#define FLAG_IMEI2              4
#define FLAG_CNT                5
#define FLAG_AUTO               6

#define IMEI_LEN                15
#define MAC_LEN                 6

class CSettingBase
{

public:
    CSettingBase(){ };
    static std::string wstr2str(const std::wstring& arg)
    {
    	int requiredSize;
    	requiredSize = WideCharToMultiByte(CP_ACP,0,arg.c_str(),arg.length(),NULL,0,NULL,NULL);
    	std::string res;
    	if (requiredSize<=0) {
    		res = "";
    		return res;
    	}
    	res.assign(requiredSize,'\0');
    	WideCharToMultiByte(CP_ACP,0,arg.c_str(),arg.length(),const_cast<char*>(res.data()),requiredSize,NULL,NULL);
    	return res;
    }
    static std::wstring str2wstr(const std::string& arg)
    {
    	int requiredSize;
    	requiredSize = MultiByteToWideChar(CP_ACP,0,arg.c_str(),arg.length(),NULL,0);
    	std::wstring res;
    	if (requiredSize<=0) {
    		res = L"";
    		return res;
    	}
    	res.assign(requiredSize,L'\0');
    	MultiByteToWideChar(CP_ACP,0,arg.c_str(),arg.length(),const_cast<wchar_t *>(res.data()),requiredSize);
    	return res;
    }
	virtual ~CSettingBase(){};
    virtual bool SaveToolSetting(std::wstring strConfig) = 0;
    virtual bool LoadToolSetting(std::wstring strConfig) = 0;
    virtual std::wstring GetStr(std::wstring key) = 0;
    virtual bool SetStr(std::wstring key,std::wstring &value) = 0;
    std::wstring int2str(int i)
    {
        wchar_t szTmep[32] = {0};
        swprintf(szTmep,32,TEXT("%d"),i);
        return szTmep;
    }
protected:
    std::wstring APPNAME;
    std::wstring szFileName;
    std::wstring strModulePath;
};

typedef struct{
    bool    bUsePS;
    bool    bDualPS;
    int     iBatteryGPIBCard;
    int     iChargerGPIBCard;
    int     iBatteryAddr;
    int     iChargerAddr;
    double  fBatteryVolt;
    double  fChargerVolt;
    double  fBatteryCurrent;
    double  fChargerCurrent;
}STRUCT_POWER_CONFIG,*PSTRUCT_POWER_CONFIG;
#define NVM_CALI    0
#define NVM_FIX     1
#define NVM_SEC     2
#define MAX_FLS     3
#define MAX_COLUMN  100
class CIniSettingBase:public CSettingBase
{
private:
    CIniFile    *pIniFile;
    CIniSection *pCurSection;
public:
    CIniSettingBase()
    {
        pIniFile    = NULL;
        pCurSection = NULL;
    };
	virtual ~CIniSettingBase();
    virtual bool SaveToolSetting(std::wstring strConfig) ;
    virtual bool LoadToolSetting(std::wstring strConfig) ;
    virtual std::wstring GetStr(std::wstring key) ;
    virtual bool SetStr(std::wstring key,std::wstring &value) ;
    void    ParseIntegers(std::wstring s, double *pInt, int *pLen);
    void    ParseIntegers(std::wstring s, int *pInt, int *nLen);
    void    ParseIntegers(std::wstring s, float *pInt, int *pLen);
    void    ParseStr(std::wstring s, std::wstring *pInt, int *pLen);
    void    ParseIp(std::wstring s, unsigned int *IPV4);
    void    ParseBools(std::wstring s, bool *pBool, int *pLen);
    static int MemoryCompare(std::wstring &s2,std::wstring &s1)
    {
        return s2.compare(s1);
    }

    std::wstring EncryptPassWord(std::wstring plaintext);
    std::wstring DecryptPassWord(std::wstring ciphertext);
    unsigned int GetIpv4()
    {
        unsigned int IPV4;
        ParseIp(strServer,&IPV4);
        return IPV4;
    }
    void SetIpv4(unsigned int IPV4)
    {
        TCHAR strIP[32] = {0};
        unsigned char ip0 = IPV4&0xff;
        unsigned char ip1 = (IPV4>>8)&0xff;
        unsigned char ip2 = (IPV4>>16)&0xff;
        unsigned char ip3 = (IPV4>>24)&0xff;
        swprintf(strIP,32,TEXT("%d.%d.%d.%d"),ip3,ip2,ip1,ip0);
        strServer = strIP;
    }

public:
    bool         bDebug;
    int          nLogLevel;
    std::wstring szLan;
    int          nLan;

    /*check **/
    bool         bReadInfo;  /*app action read or write**/
    /*recovery **/
    bool         bRecovery;

    /*software version check **/
    bool        bVersionCheck;      
    std::wstring strDeviceVersion;

    /*device battery check **/
    bool        bCheckDevicePower;  
    int         nPowerLimitLow;
    int         nPowerLimitHigh;

    /***/
    bool        bReadDdrInfo;
    bool        bReadFlashInfo;
    std::wstring strFlashSize;
    std::wstring strDdrSize;

    /*reboot device**/
    bool        bReboot;
    int         nNextMode;

    bool        bUseAtCom;

    /*data base config **/
    bool         bConnectDataBase;
    std::wstring strPort;
    std::wstring strServer;
    std::wstring strUserName;
    std::wstring strPassword;
    std::wstring strDataBaseTable;
    std::wstring strDataBaseName;

    std::wstring strSupportDatabse[6];
    int          nSupportDatabse;
    int          nDataBaseType; /*ms sql access and so on **/

    int          nPrimaryKey;
    bool         bItemUsed[5];
    std::wstring strItemName[5];
    std::wstring strItemFlag[5];

    /*check **/
    std::wstring strReadFlag; 
    std::wstring strReadTimeFlag; 
    /*write **/
    std::wstring strWriteFlag;
    std::wstring strWriteTimeFlag;
    /*recovery flag**/
    std::wstring strRecoveryFlag;
    std::wstring strRecoveryTimeFlag;
    /*Ui Settings**/
    std::wstring strItemStart[5];
    int          nItemCount[5];
    int          nItemSpan[5];

    /*Create Table Parms**/
    int          nColCount;
    std::wstring strTableColumnName[MAX_COLUMN];
    std::wstring strTableType[MAX_COLUMN];
    bool         nCanBeNull[MAX_COLUMN];    

};
class CIniLocalLan:public CSettingBase
{
private:
    CIniFile    *pIniFile;
    CIniSection *pCurSection;
public:
    CIniLocalLan()
    {
        pIniFile    = NULL;
        pCurSection = NULL;
    }
	virtual ~CIniLocalLan()
    {
        if(pIniFile) {
            delete pIniFile;
            /* pIniFile = NULL; **/
        }
    }
    virtual bool SaveToolSetting(std::wstring strConfig);
    virtual bool LoadToolSetting(std::wstring strConfig) ;
    virtual std::wstring GetStr(std::wstring key) ;
    std::wstring GetStr(std::wstring key,std::wstring default);

    virtual bool SetStr(std::wstring key,std::wstring &value);
public:
    bool    SetStringProc (void *pParam,void* lParam );
    bool    GetStringProc (void * pParam,void* lParam );
    bool    TreeControls(void * pParam,BOOL bSvae,int DlgId,bool bVer = false);
    void    TreeMenu(void * pParam,std::wstring strMainKeyPart);
};
TCHAR GenCD         (INT64 nImei);
bool CheckMac       (const char  *szImei,unsigned int flag);
bool CheckMac       (const TCHAR *szImei,unsigned int flag);
bool CheckImei      (const char  *szImei,unsigned int flag);
bool CheckImei      (const TCHAR *szImei,unsigned int flag);
bool CheckItemValid (const TCHAR *szItem,unsigned int flag);
bool CehckImeiAddCD (TCHAR *szImei);


#endif
