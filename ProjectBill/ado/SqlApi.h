#ifndef _SQLAPI_H_
#define _SQLAPI_H_
#include "ado2.h"
#include "./settings/SettingBase.h"
#include "./common/debug.h"
#include "./Common/utils.h"
#include <vector>
typedef struct  
{
    wstring item;
    int     flag;
} SItem;
typedef struct  
{
    wstring m_strItem[FLAG_CNT];
} RECORD;
/*
*class for sql connect 
*always connect to server
*
*/
class CSNWToolDataBase 
{
public:
    CSNWToolDataBase(CIniSettingBase *pConfigs,CIniLocalLan *pLan,CLogger *pLog = NULL):bIsConnected(FALSE),
        m_pConfigs(pConfigs),m_pLan(pLan),m_pLogger(pLog)
    {
        ClearItem();
    }
private:
protected:
    const CIniSettingBase *m_pConfigs;
    CIniLocalLan    *m_pLan;
    CLogger         *m_pLogger;
    BOOL            bIsConnected;
    wstring         m_strItem   [FLAG_CNT];
    BOOL            m_Item_flag [FLAG_CNT];
    unsigned char   m_Flag;
    wstring         m_strServer;    /*Server IP     **/
    wstring         m_strDataBase;  /*database name **/
    wstring         m_strUser;      /*User ID       **/
    wstring         m_strPassword;  /*UserPassword  **/
    wstring         m_strTable;     /*Table Name    **/
    wstring         m_strLastError;
    DWORD           m_dwLastError;
public:
    void StartLog(CLogger *pLogger) {
        m_pLogger = pLogger;
    }
    void StopLog(){
        m_pLogger = NULL;
    }
    std::wstring GetLocalString(std::wstring strKey)
    {
        if(m_pLan) {
            return m_pLan->GetStr(strKey,strKey);
        }
        return TEXT("");
    }
    wstring SystemTime2Str(SYSTEMTIME &sTime,int flag = 1)
    {
        TCHAR strTime[128] = {0};
        if((0 == sTime.wYear)&&(0 == sTime.wMonth)) return strTime;
        if(1 == flag){
            swprintf(strTime,dim(strTime),TEXT("%d:%d:%d")          ,sTime.wHour,sTime.wMinute,sTime.wSecond);
        } else if(2 == flag){
            swprintf(strTime,dim(strTime),TEXT("%d/%d %d:%d:%d")    ,sTime.wMonth,sTime.wYear,sTime.wHour,sTime.wMinute,sTime.wSecond);
        } else if(3 == flag) {
            swprintf(strTime,dim(strTime),TEXT("%d/%d/%d %d:%d:%d") ,sTime.wDay,sTime.wMonth,sTime.wYear,sTime.wHour,sTime.wMinute,sTime.wSecond);
        }
        return strTime;
    }

public:
    wstring TypeToStr(DWORD type){
        return m_pConfigs->strItemName[type];
    }
    wstring GetLastErr()
    {
        return m_strLastError;
    }
    BOOL IsItemValid(int type) 
    {
        return m_Item_flag[type];
    }
    wstring GetItem(int type) 
    {
        return m_strItem[type];
    }
    void ClearItem()
    {
        for(int i = 0;i<FLAG_CNT;i ++ ) {
            m_Item_flag[i] = false;
            m_strItem[i].clear();
        }
    }
    wstring GetSN()
    {
        return m_strItem[FLAG_SN];
    }
    wstring GetWifiMac()
    {
        return m_strItem[FLAG_WIFIMAC];
    }
    wstring GetBtMac()
    {
    return m_strItem[FLAG_BTMAC];  
    }
    wstring GetImei1()
    {
        return m_strItem[FLAG_IMEI1];
    }
    wstring GetImei2()
    {
        return m_strItem[FLAG_IMEI2];
    }

};
class CSNWToolSqlDB:public CSNWToolDataBase
{
public:
    CSNWToolSqlDB(CIniSettingBase *pConfigs,CIniLocalLan *pLan,CLogger *pLog = NULL):
        CSNWToolDataBase(pConfigs,pLan,pLog)
    {
        m_pDb       = NULL;
        m_pRcdSet   = NULL;
    }
    ~CSNWToolSqlDB() 
    {
        Close();
    }
private:
    CADODatabase    *m_pDb;
    CADORecordset   *m_pRcdSet;
public:
    int i;
    BOOL Open(BOOL bReOpen)
    {
        if(!m_pConfigs) return FALSE;
        return Open(m_pConfigs->strServer.c_str(),
                m_pConfigs->strDataBaseName.c_str(),
                m_pConfigs->strUserName.c_str(),
                m_pConfigs->strPassword.c_str(),
                m_pConfigs->strDataBaseTable.c_str(),bReOpen);
    }
    BOOL Open(const TCHAR *Server,const TCHAR *DataBase,const TCHAR *User,const TCHAR *Password,const TCHAR *Table,BOOL bReOpen) 
    {
        m_strServer     = Server;
        m_strDataBase   = DataBase;
        m_strUser       = User;
        m_strPassword   = Password;
        m_strTable      = Table;
        if(m_pDb&&!bReOpen) return TRUE;
        if(!m_pDb) {
            m_pDb = new CADODatabase();
        }
        if(!m_pDb) return FALSE;
        wstring strConnection = TEXT("Provider=SQLOLEDB;Server=")+ m_strServer +
                        TEXT(";DATABASE=")  + m_strDataBase+
                        TEXT(";UID=")       + m_strUser +
                        TEXT(";PWD=")       + m_strPassword + TEXT(";");
        if(m_pDb->Open(strConnection.c_str())) {	
            return TRUE;
        } 
        delete m_pDb;
        m_pDb = NULL;
        return FALSE;
    }
    BOOL Close()
    {
        if(m_pRcdSet) {
            m_pRcdSet->Close();
        }
        delete m_pRcdSet;
        if(!m_pDb) return FALSE;
        m_pDb->Close();
        delete m_pDb;
        return TRUE;
    }
    BOOL GetFieldInfo(vector<CADOFieldInfo> &vfield)
    {
        CADOFieldInfo   field;
        CADORecordset   adoRecord(m_pDb);
        if(!adoRecord.Open(m_strTable.c_str(), CADORecordset::openTable)) {
            return FALSE;
        }
        int FieldCount = adoRecord.GetFieldCount();
        for(int idx = 0; idx < FieldCount; idx ++ ) {
            if(adoRecord.GetFieldInfo(idx,&field))
            {
                vfield.push_back(field);
            }
        }
        return TRUE;

    }
    wstring GetWriteFlag()
    {
        if(m_pConfigs->bReadInfo) {
            return m_pConfigs->strReadFlag;
        } else {
            return m_pConfigs->strWriteFlag;
        }
        
    }
    wstring GetCheckFlag()
    {
        return m_pConfigs->strReadFlag;
    }    
    wstring GetMTimeFlag()
    {
        if(m_pConfigs->bReadInfo) {
            return m_pConfigs->strReadTimeFlag;
        } else {
            return m_pConfigs->strWriteTimeFlag;
        }
    }    
    wstring GetItemStr()
    {
        wstring item;
        int     i;
        for(i = 0,item.clear(); i < FLAG_CNT;i ++ ) {
            if(m_pConfigs->bItemUsed[i]) {
                if(item.empty()) {
                    item = m_pConfigs->strItemName[i];
                } else {
                    item += TEXT(",") + m_pConfigs->strItemName[i];
                }
            }
        }

        /*check flag & time**/
        if(m_pConfigs->bReadInfo) {      
            if(!m_pConfigs->strReadFlag.empty()) {
                if(item.empty()) {
                    item = m_pConfigs->strReadFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strReadFlag;
                }
            }
            if(!m_pConfigs->strReadTimeFlag.empty()) {
                if(item.empty()) {
                    item = m_pConfigs->strReadTimeFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strReadTimeFlag;
                }
            }
            
        } else { /*write flag & time**/
            if(!m_pConfigs->strWriteFlag.empty()) {
                if(item.empty()) {
                    item =  m_pConfigs->strWriteFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strWriteFlag;
                }
            }
        }

        /*we always need write flag **/
        if(!m_pConfigs->strWriteTimeFlag.empty()) {
            if(item.empty()) {
                item =  m_pConfigs->strWriteTimeFlag;
            } else {
                item += TEXT(",") + m_pConfigs->strWriteTimeFlag;
            }
        }

        /*recovery flag **/
        if(m_pConfigs->bRecovery) {
            if(!m_pConfigs->strRecoveryFlag.empty()) {
                if(item.empty()) {
                    item =  m_pConfigs->strRecoveryFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strRecoveryFlag;
                } 
            }
            if(!m_pConfigs->strRecoveryTimeFlag.empty()){
                if(item.empty()) {
                    item =  m_pConfigs->strRecoveryTimeFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strRecoveryTimeFlag;
                }
            }
        }


        
        return item;
    }
    wstring GenerateCodition(const TCHAR *strItem,DWORD type)
    {
        /*SN='rockchip15083128' **/
        return TypeToStr(type) + TEXT("='") + strItem + TEXT("'");
    }
    BOOL GetRecordForWriting(const TCHAR *strItem,DWORD type,DWORD dwflags)
    {
        TCHAR           szPrompt[128];
        wstring         Codition,Items;
        TCHAR           strSqlQuery[512];
        if(!Open(FALSE)) {
            m_strLastError = GetLocalString(TEXT("IDS_DB_CONNECT_FAIL"));
            return FALSE;
        }
        if(!m_pRcdSet) {
            m_pRcdSet = new CADORecordset(m_pDb);
        } else {
            m_pRcdSet->Close();
        }
        Codition   = GenerateCodition(strItem,type);    /*SN='rockchip15083128' **/
        Items      = GetItemStr();                      /*SN WFMAC BTMAC IMEI1 IMEI2 **/
        swprintf(strSqlQuery,sizeof(strSqlQuery)/sizeof(strSqlQuery[0]),TEXT("select %s from %s where %s"),
            Items.c_str(),
            m_strTable.c_str(),
            Codition.c_str());
        if(m_pRcdSet->Open(strSqlQuery, CADORecordset::openQuery)) {
            if(0 == m_pRcdSet->GetRecordCount()) {
                swprintf(szPrompt,dim(szPrompt),GetLocalString(TEXT("IDS_DB_RECORD_NOFIND")).c_str(),
                    TypeToStr(type).c_str(),strItem);
                m_strLastError  = szPrompt;
                return FALSE;
            }
            for(int i = 0;i < FLAG_CNT; i ++ ) {
                if(m_pConfigs->bItemUsed[i]) {
                    if(m_pRcdSet->GetFieldValue(m_pConfigs->strItemName[i].c_str(),m_strItem[i])) {
                        m_Item_flag[i] = TRUE;
                    } else {
                        swprintf(szPrompt,dim(szPrompt),
                            GetLocalString(TEXT("IDS_DB_GET_SS_FAIL")).c_str(),
                            TypeToStr(i).c_str());
                        m_strLastError  = szPrompt;
                        m_Item_flag[i] = FALSE;
                        return FALSE;
                    }
                }
            }
            if(!m_pRcdSet->GetFieldValue(GetWriteFlag().c_str(),m_Flag)) {
                swprintf(szPrompt,dim(szPrompt),
                    GetLocalString(TEXT("IDS_DB_GET_SS_FAIL")).c_str(),
                    GetWriteFlag().c_str());
                m_strLastError  = szPrompt;
                return FALSE;
            }
            if(1 == m_Flag) { /*item already used **/
                SYSTEMTIME      sTime;
                wstring         szTime;
                m_pRcdSet->GetFieldValue(GetMTimeFlag().c_str(),sTime);
                swprintf(szPrompt,dim(szPrompt),GetLocalString(TEXT("IDS_DB_RECORD_USED")).c_str(),GetWriteFlag().c_str());
                szTime = SystemTime2Str(sTime,3);
                if(szTime.empty()) {
                    m_strLastError  = szPrompt;
                } else {
                    m_strLastError  = szPrompt + wstring(TEXT("(")) + szTime + TEXT(")");
                }
                return FALSE;
            }
        } else {
            swprintf(szPrompt,dim(szPrompt),
                GetLocalString(TEXT("IDS_DB_GET_RECORD_FAIL")).c_str());
            m_strLastError  = szPrompt;
            return FALSE;
        }
        return TRUE;
    }
    BOOL OpenTableForAddNew()
    {
        TCHAR           szPrompt[128];
        wstring         Codition,Items;
        TCHAR           strSqlQuery[512];
        if(!Open(FALSE)) {
            m_strLastError = GetLocalString(TEXT("IDS_ERROR_DB_CONNECT_FAIL"));
            return FALSE;
        }
        if(!m_pRcdSet) {
            m_pRcdSet = new CADORecordset(m_pDb);
        } else {
            m_pRcdSet->Close();
        }
        if(!m_pRcdSet->Open(m_pConfigs->strDataBaseTable.c_str(), CADORecordset::openTable)) {
            return FALSE;
        }
        return TRUE;

    }
    BOOL GetRecordForChecking(const TCHAR *strItem,DWORD type,DWORD dwflags)
    {
        TCHAR           szPrompt[128];
        wstring         Codition,Items;
        TCHAR           strSqlQuery[512];
        if(!Open(FALSE)) {
            m_strLastError = GetLocalString(TEXT("IDS_ERROR_DB_CONNECT_FAIL"));
            return FALSE;
        }
        if(!m_pRcdSet) {
            m_pRcdSet = new CADORecordset(m_pDb);
        } else {
            m_pRcdSet->Close();
        }
        Codition   = GenerateCodition(strItem,type);    /*SN='rockchip15083128' **/
        Items      = GetItemStr();                      /*SN WFMAC BTMAC IMEI1 IMEI2 **/
        swprintf(strSqlQuery,sizeof(strSqlQuery)/sizeof(strSqlQuery[0]),TEXT("select %s from %s where %s"),
            Items.c_str(),
            m_strTable.c_str(),
            Codition.c_str());
        if(m_pRcdSet->Open(strSqlQuery, CADORecordset::openQuery)) {
            if(0 == m_pRcdSet->GetRecordCount()) {
                swprintf(szPrompt,dim(szPrompt),GetLocalString(TEXT("IDS_DB_RECORD_NOFIND")).c_str(),
                    TypeToStr(type).c_str(),strItem);
                m_strLastError  = szPrompt;
                return FALSE;
            }            
            for(int i = 0;i < FLAG_CNT; i ++ ) {
                if(m_pConfigs->bItemUsed[i]) {
                    if(m_pRcdSet->GetFieldValue(m_pConfigs->strItemName[i].c_str(),m_strItem[i])) {
                        m_Item_flag[i] = TRUE;
                    } else {
                        swprintf(szPrompt,dim(szPrompt),
                            GetLocalString(TEXT("IDS_ERROR_DB_GET_SS_FAIL")).c_str(),
                            TypeToStr(i).c_str());
                        m_strLastError  = szPrompt;
                        m_Item_flag[i] = FALSE;
                        return FALSE;
                    }
                }
            }
            if(!m_pRcdSet->GetFieldValue(GetWriteFlag().c_str(),m_Flag)) {
                swprintf(szPrompt,dim(szPrompt),
                    GetLocalString(TEXT("IDS_ERROR_DB_GET_SS_FAIL")).c_str(),
                    GetWriteFlag().c_str());
                m_strLastError  = szPrompt;
                return FALSE;
            }
            if(0 == m_Flag) { /*item have not been used **/
                swprintf(szPrompt,dim(szPrompt),GetLocalString(TEXT("IDS_DB_RECORD_NOWRITE")).c_str(),GetWriteFlag().c_str());
                m_strLastError  = szPrompt;
                return FALSE;
            }
        } else {
            swprintf(szPrompt,dim(szPrompt),
                GetLocalString(TEXT("IDS_ERROR_DB_GET_SS_FAIL")).c_str(),
                GetItemStr().c_str());
            m_strLastError  = szPrompt;
            return FALSE;
        }
        return TRUE;
    }
    wstring GetCommitResultItemStr()
    {
        wstring item = TEXT("");
        if(m_pConfigs->bReadInfo) { /*check flag & time**/
            if(!m_pConfigs->strReadFlag.empty()){
                if(item.empty()) {
                    item =  m_pConfigs->strReadFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strReadFlag;
                }
            }
            if(!m_pConfigs->strReadTimeFlag.empty()){
                if(item.empty()) {
                    item =  m_pConfigs->strReadTimeFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strReadTimeFlag;
                }
            }
        } else {                    /*write flag & time**/
            if(!m_pConfigs->strWriteFlag.empty()){
                if(item.empty()) {
                    item =  m_pConfigs->strWriteFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strWriteFlag;
                }
            }
            if(!m_pConfigs->strWriteTimeFlag.empty()){
                if(item.empty()) {
                    item =  m_pConfigs->strWriteTimeFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strWriteTimeFlag;
                }
            }
        }
        if(m_pConfigs->bRecovery) { /*recovery flag & time **/
            if(!m_pConfigs->strRecoveryFlag.empty()){
                if(item.empty()) {
                    item =  m_pConfigs->strRecoveryFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strRecoveryFlag;
                }
            }
            if(!m_pConfigs->strRecoveryTimeFlag.empty()){
                if(item.empty()) {
                    item =  m_pConfigs->strRecoveryTimeFlag;
                } else {
                    item += TEXT(",") + m_pConfigs->strRecoveryTimeFlag;
                }
            }
        }
        return item;
    }
    BOOL CommitResult(int flag = 1)
    {
        SYSTEMTIME  Time={0};
        GetLocalTime(&Time);
        if(!m_pRcdSet) {
            goto commiteesult_exit;
        }
        if(!m_pRcdSet->IsOpen()) {
            goto commiteesult_exit;
        }
        m_pRcdSet->Edit();
        if(m_pConfigs->bReadInfo) { /*check flag & time**/
            if(!m_pConfigs->strReadFlag.empty()){
                if(!m_pRcdSet->SetFieldValue(m_pConfigs->strReadFlag.c_str(),flag)) {
                    goto commiteesult_exit;
                }
            }
            if(!m_pConfigs->strReadTimeFlag.empty()){
                if(!m_pRcdSet->SetFieldValue(m_pConfigs->strReadTimeFlag.c_str(),Time)) {
                    goto commiteesult_exit;
                }
            }
        } else {                    /*write flag & time**/
            if(!m_pConfigs->strWriteFlag.empty()){
                if(!m_pRcdSet->SetFieldValue(m_pConfigs->strWriteFlag.c_str(),flag)) {
                    goto commiteesult_exit;
                }
            }
            if(!m_pConfigs->strWriteTimeFlag.empty()){
                if(!m_pRcdSet->SetFieldValue(m_pConfigs->strWriteTimeFlag.c_str(),Time)) {
                    goto commiteesult_exit;
                }
            }
        }
        if(m_pConfigs->bRecovery) { /*recovery flag & time **/
            if(!m_pConfigs->strRecoveryFlag.empty()){
                if(!m_pRcdSet->SetFieldValue(m_pConfigs->strRecoveryFlag.c_str(),flag)) {
                    goto commiteesult_exit;
                }
            }
            if(!m_pConfigs->strRecoveryTimeFlag.empty()){
                if(!m_pRcdSet->SetFieldValue(m_pConfigs->strRecoveryTimeFlag.c_str(),Time)) {
                    goto commiteesult_exit;
                }
            }
        }
        if(!m_pRcdSet->Update()) {
            goto commiteesult_exit;
        }
        ClearItem();
        return TRUE;
    commiteesult_exit:
        return FALSE;
    }
    BOOL AddItems2Database(vector<RECORD> &vsRecord)
    {
        if(!m_pRcdSet) {
            goto additem2database_exit;
        }
        if(!m_pRcdSet->IsOpen()) {
            goto additem2database_exit;
        }
        for(size_t i = 0; i < vsRecord.size();i ++){
            if(!m_pRcdSet->AddNew()) {
                goto additem2database_exit;
            }
            for(int j = FLAG_SN; j  < FLAG_CNT; j ++ ) {
                if(vsRecord[i].m_strItem[j].empty()) continue;
                if(!m_pRcdSet->SetFieldValue(TypeToStr(j).c_str(),vsRecord[i].m_strItem[j])) {
                    goto additem2database_exit;
                }
            }
        }
        if(!m_pRcdSet->Update()) {
            goto additem2database_exit;
        }
        return TRUE;
    additem2database_exit:
        return FALSE;
    }
    BOOL AddSingleItem2Database(RECORD&vsRecord)
    {
        if(!m_pRcdSet) {
            goto additem2database_exit;
        }
        if(!m_pRcdSet->IsOpen()) {
            goto additem2database_exit;
        }

        if(!m_pRcdSet->AddNew()) {
            goto additem2database_exit;
        }
        for(int j = FLAG_SN; j  < FLAG_CNT; j ++ ) {
            if(vsRecord.m_strItem[j].empty()) continue;
            if(!m_pRcdSet->SetFieldValue(TypeToStr(j).c_str(),vsRecord.m_strItem[j])) {
                goto additem2database_exit;
            }
        }
        if(!m_pRcdSet->Update()) {
            goto additem2database_exit;
        }
        return TRUE;
    additem2database_exit:
        return FALSE;
    }

    BOOL AddSingleItem2Database(vector<SItem> &vsItem)
    {
        if(!m_pRcdSet) {
            goto additem2database_exit;
        }
        if(!m_pRcdSet->IsOpen()) {
            goto additem2database_exit;
        }
        for(size_t i = 0; i < vsItem.size();i ++){
            if(!m_pRcdSet->AddNew()){
                goto additem2database_exit;
            }
            if(!m_pRcdSet->SetFieldValue(TypeToStr(vsItem[i].flag).c_str(),vsItem[i].item)){
                goto additem2database_exit;
            }
        }
        if(!m_pRcdSet->Update()) {
            goto additem2database_exit;
        }
        return TRUE;
    additem2database_exit:
        return FALSE;
    }
    BOOL PostRecord(TCHAR *strItem,DWORD type,DWORD dwflags)
    {
        return TRUE;
    }
    BOOL CommitWrite(int flag = 1)
    {
        return CommitWrite(m_pConfigs->strWriteFlag.c_str(),flag,m_pConfigs->strWriteTimeFlag.c_str());
    }
    BOOL CommitRead(int flag = 1)
    {
        return CommitRead(m_pConfigs->strReadFlag.c_str(),flag);
    }
    BOOL CommitRead(const TCHAR *strFlag,int flag)
    {
        if(!m_pRcdSet) {
            goto commitwrite_exit;
        }
        if(!m_pRcdSet->IsOpen()) {
            goto commitwrite_exit;
        }
        m_pRcdSet->Edit();
        if(!m_pRcdSet->SetFieldValue(strFlag,flag)){
            goto commitwrite_exit;
        }
        if(!m_pRcdSet->Update()) {
            goto commitwrite_exit;
        }
        return TRUE;
    commitwrite_exit:
        return FALSE;
    }
    BOOL CommitWrite(const TCHAR *strFlag,int flag,const TCHAR *strTimeFlag)
    {
        SYSTEMTIME  Time={0};
        GetLocalTime(&Time);
        if(!m_pRcdSet) {
            goto commitwrite_exit;
        }
        if(!m_pRcdSet->IsOpen()) {
            goto commitwrite_exit;
        }
        m_pRcdSet->Edit();
        if(!m_pRcdSet->SetFieldValue(strFlag,flag)) {
            goto commitwrite_exit;
        }
        if(!m_pRcdSet->SetFieldValue(strTimeFlag,Time)) {
            goto commitwrite_exit;
        }
        if(!m_pRcdSet->Update()) {
            goto commitwrite_exit;
        }
        return TRUE;
    commitwrite_exit:
        return FALSE;
    }
    BOOL SetFlag(const TCHAR *strFlag,int flag)
    {   
        if(!m_pRcdSet)              return FALSE;
        if(!m_pRcdSet->IsOpen())    return FALSE;
        /*if(!m_pRcdSet->AddNew())    return FALSE;**/
        m_pRcdSet->Edit();
        if(!m_pRcdSet->SetFieldValue(strFlag,flag)) return FALSE;
        if(!m_pRcdSet->Update())    return FALSE;
        return TRUE;
    }
    BOOL GetRecord(TCHAR *strItem,DWORD type,DWORD dwflags) 
    {
        unsigned char   uflags;
        wstring         filed;
        TCHAR           strSqlQuery[512];
        CADORecordset adoRecord(m_pDb);
        filed   = TypeToStr(type);
        swprintf(strSqlQuery,sizeof(strSqlQuery)/sizeof(strSqlQuery[0]),
            TEXT("select * from %s where %s='%s'"),
            m_strTable.c_str(),
            filed.c_str(),
            strItem);
        if(adoRecord.Open(strSqlQuery, CADORecordset::openQuery)) {
            if(!adoRecord.GetFieldValue(TEXT("FLAGES"),uflags)){
                return FALSE;
            }
            for(int i = 0;i < FLAG_CNT; i ++ ) {
                m_Item_flag[i] = FALSE;
                if(uflags&(1<<i)) {
                    if(adoRecord.GetFieldValue(m_pConfigs->strItemName[i].c_str(),m_strItem[i])) {
                        m_Item_flag[i] = TRUE;
                    } else {
                        return FALSE;
                    }
                }
            }
        }
        return TRUE;

    }
    BOOL GetItemFromField(TCHAR *strItem)
    {
        int             flags;
        TCHAR           strSqlQuery[512];
        wstring         wfmac,btmac;
        CADORecordset adoRecord(m_pDb);
        //m_pDb->BeginTransaction();
        swprintf(strSqlQuery,sizeof(strSqlQuery)/sizeof(strSqlQuery[0]),TEXT("select * from %s where SN='%s'"),m_strTable.c_str(),strItem);
        if(adoRecord.Open(strSqlQuery, CADORecordset::openQuery)) {
            if(adoRecord.GetFieldValue(TEXT("FLAGES"),flags)){
            }
            adoRecord.GetFieldValue(TEXT("WFMAC"),wfmac);
            adoRecord.GetFieldValue(TEXT("BTMAC"),btmac);
        }
        //m_pDb->CommitTransaction();

        return TRUE;
    }
};
extern CSNWToolSqlDB SnwtoolDataBase;
#endif
