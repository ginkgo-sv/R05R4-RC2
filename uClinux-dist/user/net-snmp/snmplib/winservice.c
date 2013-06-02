/*
 * 
 * * Windows Service related function definitions
 * * By Raju Krishnappa(raju_krishnappa@yahoo.com)
 * *
 */  
    
#include <windows.h>
#include <tchar.h>
    
#include <stdio.h>   /* sprintf */
#include <process.h>  /* beginthreadex  */
    
#include <net-snmp/library/winservice.h>
    
    /*
     * 
     * * External global variables used here
     */ 
    
    /*
     * Application Name 
     */ 
    /*
     * This should be decalred by the application, which wants to register as
     * * windows servcie
     */ 
extern LPTSTR   g_szAppName;

    /*
     * 
     * * Declare global variable
     */ 
    
    /*
     * Flag to indicate, whether process is running as Service 
     */ 
    BOOL g_fRunningAsService = FALSE;

    /*
     * Varibale to maintain Current Service status 
     */ 
static SERVICE_STATUS ServiceStatus;

    /*
     * Service Handle 
     */ 
static SERVICE_STATUS_HANDLE hServiceStatus = 0L;

    /*
     * Service Table Entry 
     */ 
    SERVICE_TABLE_ENTRY ServiceTableEntry[] = {
    
NULL, NULL};


    /*
     * Handle to Thread, to implement Pause,Resume and stop funcitonality 
     */ 
static HANDLE   hServiceThread = NULL;  /* Thread Handle */

    /*
     * Holds calling partys Function Entry point, that should started 
     * * when entered to service mode
     */ 
static          INT(*ServiceEntryPoint) (INT Argc, LPTSTR Argv[]) = 0L;

    /*
     * 
     * * To hold Stop Function address, to be called when STOP request
     * * recived from the SCM
     */ 
static          VOID(*StopFunction) () = 0L;

    /*
     * 
     * * To register as Windows Service with SCM(Service Control Manager)
     * * Input - Service Name, Serivce Display Name,Service Description and
     * * Service startup arguments
     */ 
    
    
{
    
    
    
    
    
    
        "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";
    
    
    
    
    
    
    
        
            /*
             * Open Service Control Manager handle 
             */ 
            hSCManager =
            OpenSCManager(NULL, 
        
            
            
            
            
        
            /*
             * Generate the Command to be executed by SCM 
             */ 
            _stprintf(szServiceCommand, "%s %s", szServicePath,
                      _T("-service"));
        
            /*
             * Create the Desired service 
             */ 
            hService = CreateService(hSCManager, 
                                     NULL,      /* group member tag */
                                     NULL,      /* dependencies */
                                     NULL,      /* account */
                                     NULL);     /* password */
        
            
            
                /*
                 * Generate Error String 
                 */ 
                _stprintf(MsgErrorString, "%s %s",
                          _T("Can't Create Service"),
                          lpszServiceDisplayName);
            
            
            
        
            /*
             * Create registry entires for EventLog 
             */ 
            /*
             * Create registry Application event log key 
             */ 
            _tcscpy(szRegKey, szRegAppLogKey);
        
        
            /*
             * Create registry key 
             */ 
            if (RegCreateKey(HKEY_LOCAL_MACHINE, szRegKey, &hKey) !=
                ERROR_SUCCESS)
            
            
                       _T("Unable to create registry entires"),
                       lpszServiceDisplayName);
            
            
            
        
            /*
             * Add Event ID message file name to the 'EventMessageFile' subkey 
             */ 
            RegSetValueEx(hKey, 
                          (CONST BYTE *) szServicePath,
                          
        
            /*
             * Set the supported types flags. 
             */ 
            dwData =
            EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
            EVENTLOG_INFORMATION_TYPE;
        
                       
        
            /*
             * Close Registry key 
             */ 
            RegCloseKey(hKey);
        
            /*
             * Set Service Description String  and save startup parameters if present
             */ 
            if (lpszServiceDescription != NULL || StartUpArg->Argc > 2)
            
            
                /*
                 * Create Registry Key path 
                 */ 
                _tcscpy(szRegKey,
                        _T("SYSTEM\\CurrentControlSet\\Services\\"));
            
            
            
                /*
                 * Open Registry key 
                 */ 
                if (RegOpenKeyEx
                    (HKEY_LOCAL_MACHINE, 
                     /*
                      * Create and Set access 
                      */ 
                     &hKey) != ERROR_SUCCESS)
                
                
                           _T("Unable to create registry entires"),
                           lpszServiceDisplayName);
                
                
                
            
                /*
                 * Create description subkey and the set value 
                 */ 
                if (lpszServiceDescription != NULL)
                
                
                                   (CONST BYTE *) lpszServiceDescription,
                                   
                                   sizeof(TCHAR)) != ERROR_SUCCESS)
                    
                    
                               _T("Unable to create registry entires"),
                               lpszServiceDisplayName);
                    
                    
                    
                
            
                /*
                 * Save startup arguments if they are present 
                 */ 
                if (StartUpArg->Argc > 2)
                
                
                    /*
                     * Create Subkey parameters 
                     */ 
                    if (RegCreateKeyEx
                        (hKey, "Parameters", 0, NULL,
                         
                         &hParamKey, NULL) != ERROR_SUCCESS)
                    
                    
                               _T("Unable to create registry entires"),
                               lpszServiceDisplayName);
                    
                    
                    
                
                    /*
                     * Save parameters 
                     */ 
                    
                    /*
                     * Loop through arguments 
                     */ 
                    for (i = 2, j = 1; i < StartUpArg->Argc; i++, j++)
                    
                    
                    
                        /*
                         * Create registry key 
                         */ 
                        if (RegSetValueEx
                            (hParamKey, 
                             
                             
                             sizeof(TCHAR)) != ERROR_SUCCESS)
                        
                        
                                   _T("Unable to create registry entires"),
                                   lpszServiceDisplayName);
                        
                        
                        
                    
                
            
                /*
                 * Everything is set, delete hKey 
                 */ 
                RegCloseKey(hParamKey);
            
            
        
            /*
             * Ready to Log messages 
             */ 
            
            /*
             * Successfully registered as service 
             */ 
            _stprintf(MsgErrorString, "%s %s", lpszServiceName,
                      _T("- Successfully registered as Service"));
        
            /*
             * Log message to eventlog 
             */ 
            WriteToEventLog(EVENTLOG_INFORMATION_TYPE, MsgErrorString);
        
                     
    
    
        
            CloseServiceHandle(hSCManager);
        
            CloseServiceHandle(hService);
        
            RegCloseKey(hKey);
        
            RegCloseKey(hParamKey);
    



    /*
     * 
     * * Unregister the service with the  Windows SCM 
     * * Input - ServiceName
     * *
     */ 
    
{
    
    
    
    
    
        "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";
    
    
    
        
            /*
             * Open Service Control Manager 
             */ 
            hSCManager =
            OpenSCManager(NULL, 
        
            
            
                /*
                 * Error while opening SCM 
                 */ 
                
                            
            
            
        
            /*
             * Open registered service 
             */ 
            
            OpenService(hSCManager, 
        
            
            
                       lpszServiceName);
            
            
            
        
            /*
             * Query service status 
             */ 
            /*
             * If running stop before deleting 
             */ 
            if (QueryServiceStatus(hService, &sStatus))
            
            
                 || 
                
                
                    /*
                     * Shutdown the service 
                     */ 
                    ControlService(hService, SERVICE_CONTROL_STOP,
                                   &sStatus);
                
            
        
            /*
             * Delete the service  
             */ 
            if (DeleteService(hService) == FALSE)
            
            
                       lpszServiceName);
            
            
                /*
                 * Log message to eventlog 
                 */ 
                WriteToEventLog(EVENTLOG_INFORMATION_TYPE, MsgErrorString);
            
            
        
            /*
             * Service deleted successfully 
             */ 
            _stprintf(MsgErrorString, "%s %s", lpszServiceName,
                      _T("- Service deleted"));
        
            /*
             * Log message to eventlog 
             */ 
            WriteToEventLog(EVENTLOG_INFORMATION_TYPE, MsgErrorString);
        
            /*
             * Delete registry entires for EventLog 
             */ 
            _tcscpy(szRegKey, szRegAppLogKey);
        
        
        
                     
    
    
        /*
         * Delete the handles 
         */ 
        __finally 
        
            CloseServiceHandle(hService);
        
            CloseServiceHandle(hSCManager);
    



    /*
     * 
     * * To write message to Windows Event log
     * * Input - Event Type, Message string
     * *
     */ 
    
{
    
    
    
    
    
    
    
    
    
    
        return;
    
                  NULL, 
    
    
        
        
            /*
             * We are running in command mode, output the string 
             */ 
            _putts(szMessage);
        



    /*
     * 
     * * Handle command-line arguments from the user. 
     * *     Serivce related options are:
     * *     -register       - registers the service
     * *     -unregister     - unregisters the service
     * *     -service        - run as serivce
     * *     other command-line arguments are unaltered/ignored.
     * *     They should supplied as first arguments(other wise they will be ignored
     * * Return: Type indicating the option specified
     */ 
    
{
    
    
        
        
            /*
             * second argument present 
             */ 
            if (lstrcmpi(_T("-register"), argv[1]) == 0)
            
            
            
        
        else if (lstrcmpi(_T("-unregister"), argv[1]) == 0)
            
            
            
        
        else if (lstrcmpi(_T("-service"), argv[1]) == 0)
            
            
            
        
    



    /*
     * 
     * * To Display an error message describing the last system error
     * * message, along with a title passed as a parameter.
     */ 
    VOID DisplayError(LPCTSTR pszTitle) 
{
    
    
        /*
         * Build Error String 
         */ 
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      
                      
                      
    
        
        
        
    
    else
        
        
        
    



    /*
     * 
     * *  To update current service status 
     * *  Sends the current service status to the SCM. Also updates
     * *  the global service status structure.
     */ 
static          BOOL
UpdateServiceStatus(DWORD dwStatus, 
                    
{
    
    
    
        SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    
        return FALSE;
    
    
    
    
    
        
        
        
        
    
        /*
         * special cases that depend on the new state 
         */ 
        switch (dwStatus)
        
    
        
        
    
    
        
        
        
    
    
    



    /*
     * 
     * * Reports current Service status to SCM
     */ 
static          BOOL
ReportCurrentServiceStatus() 
{
    



    /*
     * 
     * * The ServiceMain function to start service.
     */ 
    VOID WINAPI ServiceMain(DWORD argc, LPTSTR argv[]) 
{
    
    
    
        /*
         * Input Arguments to function startup 
         */ 
        DWORD ArgCount = 0;
    
    
    
    
    
    
    
    
    
        /*
         * Build the Input parameters to pass to thread 
         */ 
        
        /*
         * SCM sends Service Name as first arg, increment to point
         * * arguments user specified while starting contorl agent
         */ 
        
        /*
         * Read registry parameter 
         */ 
        /*
         * Initialize count to 1 
         */ 
        ArgCount = 1;
    
        /*
         * Create Registry Key path 
         */ 
        _stprintf(szRegKey, "%s%s\\%s",
                  _T("SYSTEM\\CurrentControlSet\\Services\\"), g_szAppName,
                  "Parameters");
    
         (HKEY_LOCAL_MACHINE, szRegKey, 0, KEY_ALL_ACCESS,
          &hParamKey) == ERROR_SUCCESS)
        
        
            /*
             * Read startup Configuration information 
             */ 
            /*
             * Find number of subkeys inside parameters 
             */ 
            
                 (hParamKey, NULL, NULL, 0, NULL, NULL, NULL, &TotalParams,
                  NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
            
            
                
                
                
                    /*
                     * Allocate memory to hold strings 
                     */ 
                    ArgArray =
                    (LPTSTR *) malloc(sizeof(LPTSTR) * ArgCount);
                
                    /*
                     * Copy first argument 
                     */ 
                    ArgArray[0] = _tcsdup(argv[0]);
                
                    
                    
                        /*
                         * Create Subkey value name 
                         */ 
                        _stprintf(szRegKey, "%s%d", "Param", i);
                    
                        /*
                         * Set size 
                         */ 
                        nSize = 128;
                    
                                     (LPBYTE) & szValue, &nSize);
                    
                    
                
            
        
        
    
        
        
            /*
             * No statup agrs are given 
             */ 
            ThreadInputParams.Argc = argc;
        
        
    
    else
        
        
        
        
    
        /*
         * Register Serivce Control Handler 
         */ 
        hServiceStatus =
        RegisterServiceCtrlHandler(g_szAppName, 
    
        
        
                         
        
        
    
        /*
         * Update the service status to START_PENDING 
         */ 
        UpdateServiceStatus(SERVICE_START_PENDING, 
                            
    
        /*
         * Spin of worker thread, which does majority of the work 
         */ 
        
        
            
            
                             
            
            
        
            (void *) _beginthreadex(&SecurityAttributes, 
                                    
                                    
                                    
        
            
            
                             
            
            
        
            /*
             * Set Service Status to Running 
             */ 
            UpdateServiceStatus(SERVICE_RUNNING, 
                                
        
            /*
             * Wait for termination event and worker thread to
             * * spin down.
             */ 
            
    
    
        
            /*
             * Release resources 
             */ 
            UpdateServiceStatus(SERVICE_STOPPED, 
                                
        
            
        
        
            /*
             * Delete allocated argument list 
             */ 
            if (ArgCount > 1 && ArgArray != NULL)
            
            
                /*
                 * Delete all strings 
                 */ 
                for (i = 0; i < ArgCount; i++)
                
                
                
            
            
    



    /*
     * 
     * * Function to start as Windows service
     * * The calling party should specify their entry point as input parameter
     * * Returns TRUE if the Service is started successfully
     */ 
    
{
    
        /*
         * Set the ServiceEntryPoint 
         */ 
        ServiceEntryPoint = ServiceFunction;
    
        /*
         * By default, mark as Running as a service 
         */ 
        g_fRunningAsService = TRUE;
    
        /*
         * Initialize ServiceTableEntry table 
         */ 
        ServiceTableEntry[0].lpServiceName = g_szAppName;       /* Application Name */
    
        /*
         * Call SCM via StartServiceCtrlDispatcher to run as Service 
         * * If the function returns TRUE we are running as Service, 
         */ 
        
        
        
        
            /*
             * Some other error has occurred. 
             */ 
            WriteToEventLog(EVENTLOG_ERROR_TYPE,
                            
                            g_szAppName);
        
    



    /*
     * 
     * * Service control handler function
     * * Responds to SCM commands/requests
     * * The service handles 4 commands
     * * commands - interrogate,pause, continue and stop.
     */ 
    VOID WINAPI ControlHandler(DWORD dwControl) 
{
    
        
    
        
        
    
        
        
    
        
        
    
        
        
        



    /*
     * 
     * * To stop the service.  This invokes registered
     * * stop function to stop the service(gracefull exit)
     * * After stopping, Service status is set to STOP in 
     * * main loop
     */ 
    VOID ProcessServiceStop(VOID) 
{
    
                         
    
        /*
         * Invoke registered Stop funciton 
         */ 
        if (StopFunction != NULL)
        
        
        
    
    else
        
        
            /*
             * There is no registered stop function, so terminate the thread 
             */ 
            TerminateThread(hServiceThread, 0);
        



    /*
     * 
     * * Returns the current state of the service to the SCM.
     */ 
    VOID ProcessServiceInterrogate(VOID) 
{
    



    /*
     * 
     * * To Create a security descriptor with a NULL ACL, which
     * * allows unlimited access. Returns a SECURITY_ATTRIBUTES
     * * structure that contains the security descriptor.
     * * The structure contains a dynamically allocated security
     * * descriptor that must be freed; either manually, or by
     * * calling FreeSecurityAttributes 
     */ 
    BOOL SetSimpleSecurityAttributes(SECURITY_ATTRIBUTES * pSecurityAttr) 
{
    
    
    
        /*
         * If an invalid address passed as a parameter, return
         * * FALSE right away. 
         */ 
        
        return FALSE;
    
        (SECURITY_DESCRIPTOR *) LocalAlloc(LPTR,
                                           
    
        return FALSE;
    
        InitializeSecurityDescriptor(pSecurityDesc,
                                     
    
        
        
            SetSecurityDescriptorDacl(pSecurityDesc, TRUE, NULL, FALSE);
        
    
        
        
        
        
        
    
    else
        
        
            /*
             * Couldn't initialize or set security descriptor. 
             */ 
            LocalFree(pSecurityDesc);
        
    



    /*
     * 
     * * This funciton Frees the security descriptor owned by a SECURITY_ATTRIBUTES
     * * structure.
     */ 
    VOID FreeSecurityAttributes(SECURITY_ATTRIBUTES * pSecurityAttr) 
{
    
        



    /*
     * TheadFunction
     * * This function is spawn as thread.
     * * Invokes registered service function
     * * Returns when called registered function returns
     */ 
    DWORD WINAPI ThreadFunction(LPVOID lpParam) 
{
    
        /*
         * lpParam contains argc and argv, pass to service main function 
         */ 
        
        /*
         * Declare pointer to InputParams 
         */ 
        InputParams * pInputArg;
    
    



    /*
     * 
     * * To register STOP function with the framework
     * * This function will be inovked when SCM sends
     * * STOP command
     */ 
    
{
    


    /*
     * 
     * * To Pause the service whec SCM sends pause command
     * * Invokes PauseThread on worker Thread handle, only
     * * when Service status is Running
     */ 
    VOID ProcessServicePause(VOID) 
{
    
        
        
                             
        
            /*
             * Invoke Thread pause on ThreadHandle 
             */ 
            if (SuspendThread(hServiceThread) != -1)
            
            
                                 
            
        



    /*
     * 
     * * To Continue paused service
     * * Invoke ResumeThread, if thread is paused
     */ 
    VOID ProcessServiceContinue(VOID) 
{
    
        
        
                             
        
            /*
             * Invoke Thread pause on ThreadHandle 
             */ 
            if (ResumeThread(hServiceThread) != -1)
            
            
                                 
            
        


