/**
 * License - MIT.
 * 
 * Module Name:
 *      wmiService.cpp
 * 
 * Abstract:
 *      WNI service class.
 * 
 * Reference:
 * https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer
*/


#include "wmiService.h"

#pragma comment(lib, "wbemuuid.lib")


/**
 * easy_wmi_init - 
*/
int EasyWMIAPI::
easy_wmi_init()
{
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres)) {
        goto err_init;              // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

                      
    if (FAILED(hres)) {
        goto err_coinit;                    // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0, 
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres)) {
        goto err_coinit;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (for example, Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );

    if (FAILED(hres)) {
        goto err_conn;                // Program has failed.
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

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

    if (FAILED(hres)) {
        goto err_proxy;               // Program has failed.
    }

    return 0;

err_proxy:
    pSvc->Release();

err_conn:
    pLoc->Release();

err_coinit:
    CoUninitialize();

err_init:
    return -1;
}

/**
 * easy_wmi_clear - 
*/
int EasyWMIAPI::
easy_wmi_clear()
{
    // Cleanup
    // ========
    if (NULL != pSvc) {
        pSvc->Release();
        pSvc = NULL;
    }

    if (NULL != pLoc) {
        pLoc->Release();
        pLoc = NULL;
    }

    if (NULL != pEnumerator) {
        pEnumerator->Release();
        pEnumerator = NULL;
    }

    CoUninitialize();

    return 0;
}

/**
 * easy_wmi_query - 
*/
int EasyWMIAPI::
easy_wmi_query(std::wstring wmiClassName, std::wstring itemId, LPVARIANT vtProp)
{
    int count = 0;
    HRESULT hres;
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;
    std::wstring queryInfo(L"SELECT ");
    queryInfo.append(itemId.c_str()).append(L" FROM ").append(wmiClassName.c_str());

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t(queryInfo.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        goto err_query;                 // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    while (pEnumerator)
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if(0 == uReturn) {
            break;
        }

        // Get the value of the Name property
        if (NULL != vtProp)
            hres = pclsObj->Get(itemId.c_str(), 0, &vtProp[count], 0, 0);

        count++;

        pclsObj->Release();
    }

    return count;

err_query:
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return -1;
}
