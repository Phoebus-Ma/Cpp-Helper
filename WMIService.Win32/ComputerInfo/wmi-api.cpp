/**
 * Win32 get computer information by WMI service.
 * Ref 1: [https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer].
 * Ref 2: [https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/computer-system-hardware-classes].
 * 
 * Example for Powershell:
 * [Get-WmiObject -Class Win32_Processor | select *].
 * 
 * License - MIT.
*/

#include "wmi-api.h"


#pragma comment(lib, "wbemuuid.lib")


IWbemLocator *pLoc  = NULL;
IWbemServices *pSvc = NULL;
IEnumWbemClassObject* pEnumerator = NULL;


/**
 * wmi_query - Query local data from WMI.
*/
int wmi_query(wstring wmiClassName, wstring itemKey, LPVARIANT vtProps)
{
    int count                   = 0;
    ULONG uReturn               = 0;
    HRESULT hres                = -1;
    IWbemClassObject* pclsObj   = NULL;

    wstring queryInfo(L"SELECT ");
    queryInfo.append(itemKey.c_str()).append(L" FROM ").append(wmiClassName.c_str());

    /**
     * Step 6:
     * Use the IWbemServices pointer to make requests of WMI.
    */
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t(queryInfo.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator
    );

    if (FAILED(hres))
    {
        goto err_query;                 // Program has failed.
    }

    /**
     * Step 7:
     * Get the data from the query in step 6.
    */
    while (pEnumerator)
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        /* Get data. */
        if (NULL != vtProps)
            hres = pclsObj->Get(itemKey.c_str(), 0, &vtProps[count], 0, 0);

        count++;

        pclsObj->Release();
    }

    return count;

err_query:
    wmi_clear();

    return -1;
}

/**
 * wmi_clear - Disconnect WMI and clear resource.
*/
int wmi_clear(void)
{
    /* Cleanup. */
    if (NULL != pSvc)
    {
        pSvc->Release();
        pSvc = NULL;
    }

    if (NULL != pLoc)
    {
        pLoc->Release();
        pLoc = NULL;
    }

    if (NULL != pEnumerator)
    {
        pEnumerator->Release();
        pEnumerator = NULL;
    }

    CoUninitialize();

    return 0;
}

/**
 * wmi_init - Connect WMI.
*/
int wmi_init(void)
{
    HRESULT hres;

    /**
     * Step 1:
     * Initialize COM.
    */
    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres))
    {
        goto err_init;              // Program has failed.
    }

    /**
     * Step 2:
     * Set general COM security levels.
    */
    hres =  CoInitializeSecurity(
        NULL, 
        -1,                         // COM authentication
        NULL,                       // Authentication services
        NULL,                       // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,  // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE,// Default Impersonation  
        NULL,                       // Authentication info
        EOAC_NONE,                  // Additional capabilities 
        NULL                        // Reserved
    );

                      
    if (FAILED(hres))
    {
        goto err_coinit;            // Program has failed.
    }

    /**
     * Step 3:
     * Obtain the initial locator to WMI.
    */
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0, 
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
        goto err_coinit;            // Program has failed.
    }

    /**
     * Step 4:
     * Connect to WMI through the IWbemLocator::ConnectServer method.
     * 
     * Connect to the root\cimv2 namespace with the current user
     * and obtain pointer pSvc to make IWbemServices calls.
    */
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"),   // Object path of WMI namespace
         NULL,                      // User name. NULL = current user
         NULL,                      // User password. NULL = current
         0,                         // Locale. NULL indicates current
         NULL,                      // Security flags.
         0,                         // Authority (for example, Kerberos)
         0,                         // Context object 
         &pSvc                      // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        goto err_conn;              // Program has failed.
    }

    /**
     * Step 5:
     * Set security levels on the proxy.
    */
    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities
    );

    if (FAILED(hres))
    {
        goto err_proxy;             // Program has failed.
    }

    return 0;

err_proxy:
err_conn:
err_coinit:
err_init:
    wmi_clear();

    return -1;
}
