/* ========================================================================= */
/* Window Manager                                           Mhatxotic Design */
/* ========================================================================= */
#define WIN32_LEAN_AND_MEAN            // Faster compulation of headers
#define WINVER                  0x0500 // Windows 2000 or better required
#define _WIN32_WINNT            WINVER // Windows 2000 or better required
#define _WIN32_WINDOWS          WINVER // Windows 2000 or better required
/* ========================================================================= */
#include <process.h>                   // Process and thread headers
#include <windows.h>                   // Windows sdk headers
#include <commctrl.h>                  // windows common control headers
#include <shellapi.h>                  // Windows shell api headers
#include <commdlg.h>                   // windows common dialog headers
/* ========================================================================= */
#include <list>                        // Need std::list class
#include <map>                         // Need std::map class
#include <sstream>                     // Need std::*string* classes
#include <algorithm>                   // Need std::tramsform class
#include <vector>                      // Need std::vector class
/* ========================================================================= */
#pragma comment(lib,     "User32.Lib") // User interface functions
#pragma comment(lib,      "Gdi32.Lib") // Graphical interface functions
#pragma comment(lib,   "ComDlg32.Lib") // Common dialog library functions
#pragma comment(lib,   "ComCtl32.Lib") // Common control library functions
#pragma comment(lib,    "Shell32.Lib") // Shell library functions
/* ========================================================================= */
class PROGRAM                          // Start of program class
{ /* --------------------------------------------------------------- */ public:
  class MEMORY                         // Start of memory block class
  { /* ------------------------------------------------------------ */ private:
    char          *cpPtr;              // Pointer to data
    size_t         stSize;             // Size of data
    /* --------------------------------------------------------------------- */
    void InitialiseVars(void) { cpPtr = NULL; stSize = 0; }
    /* --------------------------------------------------------------------- */
    void InitObjectFromClass(const MEMORY &memClass)
    {
      // Set size
      stSize = ((MEMORY&)memClass).Size();
      // Set pointer
      cpPtr = (char*)((MEMORY&)memClass).Ptr();
      // Init this new class' vars
      ((MEMORY&)memClass).InitialiseVars();
    }
    /* ------------------------------------------------------------- */ public:
    inline const size_t Size(void) { return stSize; }
    /* --------------------------------------------------------------------- */
    inline const char *Ptr(void) { return cpPtr; }
    /* --------------------------------------------------------------------- */
    const size_t Find(std::string strMatch)
    {
      // Bail if string invalid
      if(strMatch.length() <= 0) return std::string::npos;
      // Position and end of string
      size_t stIndex, stLoc = 0;
      // Until end of string
      while(char *cpLoc = (char*)memchr(Ptr()+stLoc, strMatch.at(0), Size()-stLoc))
      {
        // Calculate index
        stLoc = (size_t)(cpLoc-Ptr());
        // Walk data until one of three things happen
        // - End of match string
        // - Character mismatch
        // - End of memory block
        for(stIndex = 0;
            stIndex < strMatch.length() &&
            strMatch.at(stIndex) == Ptr()[stLoc+stIndex] &&
            stLoc+stIndex < Size();
          ++stIndex);
        // If we read all of the string match then we succeeded
        if(stIndex >= strMatch.length()) return stLoc;
        // Incrememnt position and try again
        stLoc += stIndex;
      }
      // Failed
      return std::string::npos;
    }
    /* --------------------------------------------------------------------- */
    void Set(const size_t stPosition, const unsigned char ucByte,
      const size_t stLen)
    {
      // Bail if addr invalid
      if(stLen <= 0) throw std::exception("write invalid params");
      // Bail if position bad
      if(stPosition >= Size()) throw std::exception("set pos invalid");
      // Bail if size bad
      if(stPosition+stLen > Size()) throw std::exception("set count invalid");
      // Set the memory
      memset((char*)Ptr()+stPosition, ucByte, stLen);
    }
    /* --------------------------------------------------------------------- */
    char *Read(const size_t stPosition)
    {
      // Bail if size bad
      if(stPosition >= Size()) throw std::exception("read pos invalid");
      // Return pointer
      return (char*)Ptr()+stPosition;
    }
    /* --------------------------------------------------------------------- */
    template<typename T>void Write(const size_t stPosition, T tSource,
      const size_t stToCopy)
    {
      // Bail if addr invalid
      if(!tSource || stToCopy <= 0) throw std::exception("write invalid params");
      // Bail if position bad
      if(stPosition >= Size()) throw std::exception("write pos invalid");
      // Bail if size bad
      if(stPosition+stToCopy > Size()) throw std::exception("write count invalid");
      // Do copy
      memcpy((char*)Ptr()+stPosition, tSource, stToCopy);
    }
    /* --------------------------------------------------------------------- */
    void Initialise(const size_t _stSize)
    {
      // Deinitialise
      Deinitialise();
      // Set size
      stSize = _stSize;
      // Allocate
      cpPtr = new char[Size()];
    }
    /* --------------------------------------------------------------------- */
    void Deinitialise(void)
    {
      // Free the pointer
      if(Ptr()) { delete []Ptr(); }
      // Reset vars
      InitialiseVars();
    }
    /* --------------------------------------------------------------------- */
    ~MEMORY(void) { Deinitialise(); }
    /* -- Assignment copy constructor -------------------------------------- */
    MEMORY &operator=(const MEMORY &memClass)
    {
      // Initialise
      InitObjectFromClass(memClass);
      // Return this
      return *this;
    }
    /* -- Copy constructor ------------------------------------------------- */
    MEMORY(const MEMORY &memClass) { InitObjectFromClass(memClass); }
    /* --------------------------------------------------------------------- */
    MEMORY(void) { InitialiseVars(); }
    /* --------------------------------------------------------------------- */
    template<typename T>MEMORY(const size_t _stSize, T tBuffer)
    {
      // Init members
      InitialiseVars();
      // Initialise space
      Initialise(_stSize);
      // Copy memory
      Write(0, tBuffer, _stSize);
    }
  };/* --------------------------------------------------------------------- */
  /* ----------------------------------------------------------------------- */
  class FSTREAM                        // Start of stream class
  { /* ------------------------------------------------------------- */ public:
    FILE          *fStream;            // Stream handle
    std::string    strFilename;        // Filename
    /* --------------------------------------------------------------------- */
    inline const size_t SetPosition(const long lPos, const int iFlags)
      { return fseek(fStream, (long)lPos, iFlags); }
    /* --------------------------------------------------------------------- */
    inline const size_t GetPosition(void) { return ftell(fStream); }
    /* --------------------------------------------------------------------- */
    inline const size_t Read(void *vpBuffer, const size_t stBytes,
      const size_t stItems)
      { return fread(vpBuffer, stBytes, stItems, fStream); }
    /* --------------------------------------------------------------------- */
    inline const size_t Write(const void *vpBuffer, const size_t stBytes,
      const size_t stItems)
      { return fwrite(vpBuffer, stBytes, stItems, fStream); }
    /* --------------------------------------------------------------------- */
    const size_t Size(void)
    {
      // Store current position
      const size_t stCurrent = GetPosition();
      // Set end psotion
      SetPosition(0, SEEK_END);
      // Store position
      const size_t stSize = GetPosition();
      // Restore position
      SetPosition((long)stCurrent, SEEK_SET);
      // Return size
      return stSize;
    }
    /* --------------------------------------------------------------------- */
    const int Open(const char *cpFile, const char *cpAccess)
    {
      // Close file
      Close();
      // Set filename
      strFilename = cpFile;
      // Open file
      return fopen_s(&fStream, cpFile, cpAccess);
    }
    /* --------------------------------------------------------------------- */
    void Close(void)
    {
      // Close stream if opened
      if(fStream != NULL) fclose(fStream);
      // Done
      InitVariables();
    }
    /* --------------------------------------------------------------------- */
    void InitVariables(void)
    {
      // Set members
      fStream = NULL;
      strFilename.clear();
    }
    /* --------------------------------------------------------------------- */
    ~FSTREAM(void) { Close(); }
    /* --------------------------------------------------------------------- */
    FSTREAM(void) { InitVariables(); }
  };/* --------------------------------------------------------------------- */
  /* ----------------------------------------------------------------------- */
  class TOKEN
  { /* ------------------------------------------------------------- */ public:
    typedef std::vector<std::string> TOKENLIST;
    /* --------------------------------------------------------------------- */
    TOKENLIST      tokenList;          // Token list
    /* --------------------------------------------------------------------- */
    TOKEN(std::string strString, std::string strSeparator)
    {
      // Location of cpSeparator
      size_t stStart = 0, stLoc;
      // Until eof
      while((stLoc = strString.find(strSeparator, stStart)) !=
       std::string::npos)
      {
        // Found
        tokenList.push_back(strString.substr(stStart, stLoc-stStart));
        // Increase position
        stStart += stLoc-stStart+strSeparator.length();
      }
      // Push remainder of string
      tokenList.push_back(strString.substr(stStart));
    }
  };/* --------------------------------------------------------------------- */
  /* ----------------------------------------------------------------------- */
  class VARS                           // Start of vars class
  { /* ------------------------------------------------------------- */ public:
    typedef std::map<std::string,std::string> VARLIST; // Var list typedef
    /* --------------------------------------------------------------------- */
    VARLIST        varList;            // Vars list
    /* --------------------------------------------------------------------- */
    std::string GetHeader(std::string strName)
    {
      // Bail if invalid header
      if(strName.length() <= 0) throw std::exception("invalid getheader name");
      // Find header
      VARLIST::iterator varItem = varList.find(strName);
      // Return state
      return varItem == varList.end() ? "" : (*varItem).second;
    }
    /* --------------------------------------------------------------------- */
    std::string Trim(std::string strSource, const unsigned char ucChar)
    {
      // Remove leading whitespaces
      while(strSource.length() > 0 && strSource.at(0) == ucChar)
        strSource.erase(0, 1);
      // Remove trailing whitespaces
      while(strSource.length() > 0 && strSource.at(strSource.length()-1)
        == ucChar) strSource.erase(strSource.length()-1, 1);
      // Return string
      return strSource;
    }
    /* --------------------------------------------------------------------- */
    void PushLine(std::string strLine, std::string strSeparator)
    {
      // Bail if invalid
      if(strLine.length() <= 0) throw std::exception("Invalid push source");
      // Bail if invalid
      if(strSeparator.length() <= 0) throw std::exception("Invalid push sep");
      // Look for separator
      const size_t stSepLoc = strLine.find(strSeparator);
      // Not found?
      if(stSepLoc == std::string::npos)
      {
        // Create unique variable
        std::ostringstream ossVar;
        // Push a unique number
        ossVar << "?" << varList.size() << "?";
        // Push string
        varList[ossVar.str()] = Trim(strLine, ' ');
        // Done
        return;
      }
      // Push variable and value
      varList[Trim(strLine.substr(0, stSepLoc), ' ')] =
        Trim(strLine.substr(stSepLoc+strSeparator.length()), ' ');
    }
    /* --------------------------------------------------------------------- */
    void Initialise(std::string strString, std::string strLineSep,
      std::string strVarSep)
    {
      // Clear existing headers
      DeInitialise();
      // Location of cpSeparator
      size_t stStart = 0, stLoc;
      // Until eof
      while((stLoc = strString.find(strLineSep, stStart)) !=
        std::string::npos)
      {
        // Push line
        PushLine(strString.substr(stStart, stLoc-stStart),
         strVarSep);
        // Increase position
        stStart = stLoc + strLineSep.length();
      }
      // Push remainder of string if available
      PushLine(strString.substr(stStart), strVarSep);
    }
    /* --------------------------------------------------------------------- */
    void DeInitialise(void) { varList.clear(); }
    /* --------------------------------------------------------------------- */
    ~VARS(void) { DeInitialise(); }
    /* --------------------------------------------------------------------- */
    VARS(void) { }
  };/* --------------------------------------------------------------------- */
  class WINDOW                         // Start of window class
  { /* ------------------------------------------------------------- */ public:
    class CONTROL                      // Start of control class
    { /* ----------------------------------------------------------- */ public:
      HWND         hwndControl;        // Handle to control
      /* ------------------------------------------------------------------- */
      CONTROL(void) { hwndControl = NULL; }
      /* ------------------------------------------------------------------- */
      CONTROL(const CONTROL &controlItem)
      {
        // Set control
        hwndControl = controlItem.hwndControl;
        // Nullify old control
        ((CONTROL&)controlItem).hwndControl = NULL;
      }
      /* ------------------------------------------------------------------- */
      const CONTROL &operator=(const CONTROL &controlItem)
      {
        // Set control
        hwndControl = controlItem.hwndControl;
        // Nullify old control
        ((CONTROL&)controlItem).hwndControl = NULL;
        // Return this
        return *this;
      }
      /* ------------------------------------------------------------------- */
      CONTROL(const size_t stId, const int iX, const int iY,
        const unsigned int uiWidth, const unsigned int uiHeight,
        const char *cpClassName, const char *cpText, const DWORD dwStyleEx,
        const DWORD dwStyle)
      {
        // Create handle
        hwndControl = CreateWindowEx(dwStyleEx, cpClassName, NULL,
          dwStyle|WS_CHILD, iX, iY, uiWidth, uiHeight,
          programClass.windowClass.hwndWindow, (HMENU)stId,
          programClass.windowClass.classData.hInstance, this);
        // Bail if not created
        if(!hwndControl) return;
        // Set font
        SendMessage(WM_SETFONT, (WPARAM)programClass.windowClass.hfontWindow,
          FALSE);
        // If we are creating a bitmap
        if(!_strcmpi(cpClassName, WC_STATIC) && dwStyle & SS_BITMAP && cpText)
        {
          // Create bitmap
          const HBITMAP hB = (HBITMAP)LoadImage(
            programClass.windowClass.classData.hInstance, cpText,
            IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
          // If succeeded?
          if(hB != NULL)
          {
            // Add to user data
            SetWindowLongPtr(hwndControl, GWLP_USERDATA, (LONG_PTR)hB);
            // Send control message
            SendMessage(STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hB);
          }
        }
        else SetText("%s",cpText);
      }
      /* ------------------------------------------------------------------- */
      inline void Enable(void) { RemoveStyle(WS_DISABLED); }
      /* ------------------------------------------------------------------- */
      inline void Disable(void) { AddStyle(WS_DISABLED); }
      /* ------------------------------------------------------------------- */
      inline void Focus(void) { SetFocus(hwndControl); }
      /* ------------------------------------------------------------------- */
      inline void AddStyle(const DWORD dwStyle)
      {
        // Add style
        SetStyle((DWORD)GetStyle() | dwStyle);
      }
      /* ------------------------------------------------------------------- */
      inline void RemoveStyle(const DWORD dwStyle)
      {
        // Add style
        SetStyle((DWORD)GetStyle() & ~dwStyle);
      }
      /* ------------------------------------------------------------------- */
      inline const LONG_PTR GetStyle(void)
      {
        // Set style
        return GetWindowLongPtr(hwndControl, GWL_STYLE);
      }
      /* ------------------------------------------------------------------- */
      inline void SetStyle(const DWORD dwStyle)
      {
        // Set style and update control
        SetWindowLongPtr(hwndControl, GWL_STYLE, dwStyle);
        InvalidateRect(hwndControl, NULL, FALSE);
        UpdateWindow(hwndControl);
      }
      /* ------------------------------------------------------------------- */
      inline const LRESULT SendMessage(const UINT uM, const WPARAM wP,
        const LPARAM lP)
      {
        // Return sent message
        return ::SendMessage(hwndControl, uM, wP, lP);
      }
      /* ------------------------------------------------------------------- */
      void SetText(std::string strFormat, ...)
      {
        // Create pointer to arguments
        va_list vlArgs;
        // Get pointer to arguments
        va_start(vlArgs, strFormat);
        // Build string
        strFormat = programClass.FormatArguments(strFormat, vlArgs);
        // Add item
        SendMessage(WM_SETTEXT, 0, (LPARAM)strFormat.c_str());
        // Clean up arguments
        va_end(vlArgs);
      }
      /* ------------------------------------------------------------------- */
      ~CONTROL(void)
      {
        // If handle not set? return
        if(!hwndControl||!IsWindow(hwndControl)) return;
        // Check for bitmap handle
        const HBITMAP hB = (HBITMAP)GetWindowLongPtr(hwndControl, GWLP_USERDATA);
        // If set? Destroy it!
        if(hB) DeleteObject(hB);
        // Destroy handle
        DestroyWindow(hwndControl);

      }
    };/* ------------------------------------------------------------------- */
    typedef std::map<UINT,CONTROL> CONTROLLIST; // Control list
    /* --------------------------------------------------------------------- */
    typedef const bool(CALLBACK *EVENTCALLBACK)(const WPARAM,const LPARAM);
    /* --------------------------------------------------------------------- */
    typedef std::map<UINT,EVENTCALLBACK> EVENTLIST; // Event list
    /* --------------------------------------------------------------------- */
    HWND           hwndWindow;         // Window handle
    WNDCLASSEX     classData;          // Window class data
    HFONT          hfontWindow;        // Window font
    HDC            hdcWindow;          // Window device context
    DWORD          dwStyle;            // Window style
    DWORD          dwStyleEx;          // Window extended style
    EVENTLIST      eventList;          // Window events list
    CONTROLLIST    controlList;        // Window control list
    /* --------------------------------------------------------------------- */
    CONTROL &GetControl(const size_t stId)
    {
      // Bail if item exists
      CONTROLLIST::iterator controlItem = controlList.find(stId);
      // If incorrect throw exception
      if(controlItem == controlList.end()) throw std::exception("invalid control");
      // Return item
      return (*controlItem).second;
    }
    /* --------------------------------------------------------------------- */
    void AddControl(const size_t stId, const int iX, const int iY,
        const unsigned int uiWidth, const unsigned int uiHeight,
        const char *cpClassName, const char *cpText, const DWORD dwStyleEx,
        const DWORD dwStyle)
    {
      // Bail if item exists
      if(controlList.find(stId) != controlList.end()) programClass.Error(2,
        "Window control already registered!");
      // Assign
      controlList[stId] = CONTROL(stId, iX, iY, uiWidth, uiHeight,
        cpClassName, cpText, dwStyleEx, dwStyle);
    }
    /* --------------------------------------------------------------------- */
    const bool RemoveEvent(const unsigned int uMsgId)
    {
      // Find event
      EVENTLIST::iterator eventItem = eventList.find(uMsgId);
      // If event not exists bail
      if(eventItem == eventList.end()) return false;
      // Remove event
      eventList.erase(eventItem);
      // Done
      return true;
    }
    /* --------------------------------------------------------------------- */
    const bool AddEvent(const unsigned int uMsgId, EVENTCALLBACK cbFunc)
    {
      // Find event
      EVENTLIST::iterator eventItem = eventList.find(uMsgId);
      // If event already exists bail
      if(eventItem != eventList.end()) return false;
      // Set event
      eventList[uMsgId] = cbFunc;
      // Done
      return true;
    }
    /* --------------------------------------------------------------------- */
    const LRESULT Cb(const HWND hH, const unsigned int uM, const WPARAM wP, const LPARAM lP)
    {
      // Window is being destroyed?
      if(uM == WM_DESTROY)
      {
        // Destroy controls
        controlList.clear();
        // Context is no longer valid
        hdcWindow = NULL;
        // Handle is no longer valid
        hwndWindow = NULL;
        // Post quit message
        PostQuitMessage(0);
      }
      // Events list has window message
      if(eventList.find(uM) != eventList.end())
        // Call the function callback
        if(eventList[uM](wP, lP) == true)
          // Return event handled
          return false;
      // Event not handled
      return DefWindowProc(hH, uM, wP, lP);
    }
    /* --------------------------------------------------------------------- */
    static LRESULT CALLBACK CbStatic(HWND hH, unsigned int uM, WPARAM wP, LPARAM lP)
    {
      // Window is not being created?
      if(uM != WM_NCCREATE)
        // Return handled event
        return reinterpret_cast<WINDOW*>(GetWindowLongPtr(hH, GWLP_USERDATA))->
          Cb(hH, uM, wP, lP);
      // Window is being created. Get class name
      WINDOW &windowClass = *(WINDOW*)((LPCREATESTRUCT)lP)->lpCreateParams;
      // Set window handle in class
      windowClass.hwndWindow = hH;
      // Set window device context
      windowClass.hdcWindow = GetDC(hH);
      // Set user data in handle to point to this class
      SetWindowLongPtr(hH, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(&windowClass));
      // Process the event
      return windowClass.Cb(hH, uM, wP, lP);
    }
    /* --------------------------------------------------------------------- */
    void Activate(void) { SetForegroundWindow(hwndWindow); }
    /* --------------------------------------------------------------------- */
    void Focus(void) { SetFocus(hwndWindow); }
    /* --------------------------------------------------------------------- */
    void Show(const int iState) { ShowWindow(hwndWindow, iState); }
    /* --------------------------------------------------------------------- */
    void MoveEnd(const int iX, const int iY)
    {
      // Desktop and window bounds
      RECT sDeskRect = { 0 }, sRect = { 0 };
      // Want working area parameters
      SystemParametersInfo(SPI_GETWORKAREA, 0, &sDeskRect, NULL);
//      AdjustWindowRectEx(&sDeskRect, dwStyle, FALSE, dwStyleEx);
//      if(sDeskRect.left < 0) sDeskRect.right += sDeskRect.left;
//      if(sDeskRect.top < 0) sDeskRect.bottom += sDeskRect.top;
      sDeskRect.left = sDeskRect.top = 0;
      // Get window bounds
      GetWindowRect(hwndWindow, &sRect);
      // Set new position info
      sRect.right -= sRect.left;
      sRect.bottom -= sRect.top;
      sRect.left = sDeskRect.right - sRect.right;
      sRect.top = sDeskRect.bottom - sRect.bottom;
      // Adjust position
      Move(sRect.left + iX, sRect.top + iY);
    }
    /* --------------------------------------------------------------------- */
    void Move(const int iX, const int iY)
    {
      // Adjust position
      SetWindowPos(hwndWindow, NULL, iX, iY, -1, -1,
        SWP_NOSIZE | SWP_NOZORDER);
    }
    /* --------------------------------------------------------------------- */
    void Centre(void)
    {
      // Get desktop bounds
      RECT sDeskRect;
      GetWindowRect(GetDesktopWindow(), &sDeskRect);
      AdjustWindowRectEx(&sDeskRect, dwStyle, FALSE, dwStyleEx);
      if(sDeskRect.left < 0) sDeskRect.right += sDeskRect.left;
      if(sDeskRect.top < 0) sDeskRect.bottom += sDeskRect.top;
      sDeskRect.left = sDeskRect.top = 0;
      // Get window bounds
      RECT sRect;
      GetWindowRect(hwndWindow, &sRect);
      // Set new position info
      sRect.right -= sRect.left;
      sRect.bottom -= sRect.top;
      sRect.left = (sDeskRect.right / 2) - (sRect.right / 2);
      sRect.top = (sDeskRect.bottom / 2) - (sRect.bottom / 2);
      // Adjust position
      Move(sRect.left, sRect.top);
    }
    /* --------------------------------------------------------------------- */
    void Resize(const int iWidth, const int iHeight)
    {
      // Rect for window bounds
      RECT sRect;
      // Fill bounds
      sRect.left = sRect.top = 0;
      sRect.right = iWidth;
      sRect.bottom = iHeight;
      // Adjust size to accomodate window style
      AdjustWindowRectEx(&sRect, dwStyle, FALSE, dwStyleEx);
      sRect.right += -sRect.left;
      sRect.bottom += -sRect.top;
      // Adjust position
      SetWindowPos(hwndWindow, 0, -1, -1, sRect.right, sRect.bottom, SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOMOVE);
      // Repaint all windows
      InvalidateRect(NULL, &sRect, FALSE);
    }
    /* --------------------------------------------------------------------- */
    void ClearFont(void)
    {
      // Delete font if created
      if(hfontWindow) DeleteObject(hfontWindow);
      // Nullify
      hfontWindow = NULL;
    }
    /* --------------------------------------------------------------------- */
    void SetFont(const char *cpFont, const int iSize)
    {
      // Destroy font if created
      ClearFont();
      // Create the font
      hfontWindow = CreateFont(iSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, cpFont);
      // Bail if failed
      if(!hfontWindow) programClass.Error(1,
        "Failed to create %dpt %s font (code:%x)", iSize, cpFont, GetLastError());
      // Select object
      if(!SelectObject(hdcWindow, hfontWindow)) programClass.Error(1,
        "Failed to select %dpt %s font into context (code:%x)", iSize, cpFont,
        GetLastError());
    }
    /* --------------------------------------------------------------------- */
    void SetWindowTextF(const HWND hwndHandle, std::string strFormat, ...)
    {
      // Create pointer to arguments
      va_list vlArgs;
      // Get pointer to arguments
      va_start(vlArgs, strFormat);
      // Build string
      strFormat = programClass.FormatArguments(strFormat, vlArgs);
      // Show error
      SetWindowText(hwndHandle, strFormat.c_str());
      // Clean up arguments
      va_end(vlArgs);
    }
    /* --------------------------------------------------------------------- */
    void Initialise(std::string strClassName, const int iIconId,
      const DWORD _dwStyleEx, const DWORD _dwStyle)
    {
      // Set styles
      dwStyle = _dwStyle;
      dwStyleEx = _dwStyleEx;
      // Initialise class members
      classData.style = CS_HREDRAW | CS_VREDRAW;
      classData.lpfnWndProc = (WNDPROC)CbStatic;
      classData.hInstance = GetModuleHandle(0);
      classData.hIcon = (HICON)LoadImage(classData.hInstance,
        MAKEINTRESOURCE(iIconId), IMAGE_ICON, GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
      classData.hIconSm = (HICON)LoadImage(classData.hInstance,
        MAKEINTRESOURCE(iIconId), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
      classData.hCursor = LoadCursor(0, IDC_ARROW);
      classData.lpszClassName = strClassName.c_str();
      classData.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
      // Register the class. Bail if failed
      if(!RegisterClassEx(&classData))
        programClass.Error(1, "Could not register window class (code:%x)",
          GetLastError());
      // Create window. Bail if failed
      if(CreateWindowEx(dwStyleEx, classData.lpszClassName,
        classData.lpszClassName, dwStyle, 1, 1, 1, 1, NULL, NULL,
        classData.hInstance, this) == NULL || hwndWindow == NULL)
          programClass.Error(2, "Could to create window handle (code:%x)",
            GetLastError());
    }
    /* --------------------------------------------------------------------- */
    void DeInitialise(void)
    {
      // Destroy font if created
      ClearFont();
      // Window handle created and really is window?
      if(hwndWindow && IsWindow(hwndWindow))
      {
        // Release context if acquired
        if(hdcWindow) ReleaseDC(hwndWindow, hdcWindow);
        // Destroy handle if really is window
        DestroyWindow(hwndWindow);
      }
      // Window class registeried?
      if(classData.cbClsExtra)
        // Unregister the class
        UnregisterClass(classData.lpszClassName, classData.hInstance);
      // Clear data
      InitVariables();
    }
    /* --------------------------------------------------------------------- */
    void InitVariables(void)
    {
      // Nullify font handle
      hfontWindow = NULL;
      // Nullify window handle
      hwndWindow = NULL;
      // Nullify device context
      hdcWindow = NULL;
      // Clear structure
      memset(&classData, '\0', sizeof(classData));
      // Set structure size
      classData.cbSize = sizeof(classData);
      // Set styles
      dwStyle = dwStyleEx = 0;
    }
    /* --------------------------------------------------------------------- */
    void Main(void)
    {
      // Create message queue structure
      MSG sMsg;
      // Until WM_QUIT or error occurs
      while(GetMessage(&sMsg, 0, 0, 0) > 0)
      {
        // Is a dialog message? Ignore it
        if(IsDialogMessage(hwndWindow, &sMsg)) continue;
        // Translate the message
        TranslateMessage(&sMsg);
        // Dispatch the message
        DispatchMessage(&sMsg);
      }
    }
    /* --------------------------------------------------------------------- */
    ~WINDOW(void)
    {
      // DeInitialise
      DeInitialise();
    }
    /* --------------------------------------------------------------------- */
    WINDOW(void)
    {
      // Initialise members
      InitVariables();
    }
    /* --------------------------------------------------------------------- */
  } windowClass;                       // Window class
  /* ----------------------------------------------------------------------- */
  class TRAYICON                     // Tray icon class
  { /* -- Typedefs ------------------------------------------------------- */
    typedef const bool (CALLBACK *TC_CALLBACK)(void); // Callback
    /* ------------------------------------------------------------------- */
    private:                         // All declarations are now private
    /* -- Structs -------------------------------------------------------- */
    typedef struct _TC_EVT           // Tray icon event structure
    {
      LPARAM     lParam;             // Tray icon click message
      TC_CALLBACK cbCallback;        // Event callback function
    }
    TC_EVT;                          // Static
    typedef std::map<LPARAM,TC_EVT> LST_EVT; // Event list
    /* --------------------------------------------------------------------- */
    NOTIFYICONDATA nidData;            // Notify icon data
    LST_EVT        listEvents;         // Click events
    UINT           uiTbRestartEvent;   // Taskbar restart event id
    /* --------------------------------------------------------------------- */
    public:                            // All declarations are now public
    /* -- Restart Callback event ------------------------------------------- */
    static const bool CALLBACK CbRestart(const WPARAM, const LPARAM)
    {
      // Simulate hiding the tray icon to reset internal variables
      programClass.trayIcon.Hide();
      // Reshow the icon
      programClass.trayIcon.Show();
      // Event was handled
      return true;
    }
    /* -- Callback event --------------------------------------------------- */
    static const bool CALLBACK Cb(const WPARAM, const LPARAM lParam)
    {
      // For each event
      for(LST_EVT::iterator itemEvent = programClass.trayIcon.listEvents.begin();
                            itemEvent != programClass.trayIcon.listEvents.end();
                          ++itemEvent)
      {
        // Get pointer to data
        TC_EVT &sEventData = (*itemEvent).second;
        // Event id matches
        if(lParam == sEventData.lParam)
          // Run callback and return status
          return sEventData.cbCallback();
      }
      // Event was not handled
      return false;
    }
    /* -- UnregisterAllEvents --------- Unregister all mouse click events -- */
    void UnregisterAllEvents(void)
    {
      // Clear list
      listEvents.clear();
    }
    /* -- SetTip ----------------------------------------------------------- */
    BOOL SetTip(char *cpFormat, ...)
    {
      // No tip?
      if(cpFormat == NULL)
        // Remove flag
        nidData.uFlags &= ~NIF_TIP;
      // Want tip
      else
      {
        // Parse the format string and fill the tip buffer
        _vsnprintf_s(nidData.szTip, sizeof(nidData.szTip), cpFormat, (va_list)(&cpFormat + 1));
        // Add flag
        nidData.uFlags |= NIF_TIP;
      }
      // Modify the icon
      return Shell_NotifyIcon(NIM_MODIFY, &nidData);
    }
    /* -- SetIcon ---------------------------------------------------------- */
    BOOL SetIcon(LPCTSTR lpszResource)
    {
      // Existing icon loaded?
      if(nidData.hIcon != NULL)
      {
        // Unload it from memory
        DeleteObject(nidData.hIcon);
        // Nullify
        nidData.hIcon = NULL;
      }
      // Remove icon?
      if(lpszResource == NULL)
        // Remove the icon flag
        nidData.uFlags &= ~NIF_ICON;
      // New icon?
      else
      {
        // Load up the icon
        nidData.hIcon = (HICON)LoadImage(programClass.windowClass.classData.hInstance, lpszResource, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
        // Add the icon flag
        nidData.uFlags |= NIF_ICON;
      }
      // Modify the icon
      return Shell_NotifyIcon(NIM_MODIFY, &nidData);
    }
    /* -- Hide ------------------------------------------------------------- */
    BOOL Hide(void)
    {
      // Unregister the event
      programClass.windowClass.RemoveEvent(nidData.uCallbackMessage);
      // Restart event registered? Unregister the event
      programClass.windowClass.RemoveEvent(uiTbRestartEvent);
      // Hide the icon
      return Shell_NotifyIcon(NIM_DELETE, &nidData);
    }
    /* -- Show ------------------------------------------------------------- */
    BOOL Show(void)
    {
      // Register monitor event
      programClass.windowClass.AddEvent(nidData.uCallbackMessage, Cb);
      // Restart event registered?
      if(uiTbRestartEvent != 0)
        // Register taskbar restart event
        programClass.windowClass.AddEvent(uiTbRestartEvent, CbRestart);
      // Show the icon
      return Shell_NotifyIcon(NIM_ADD, &nidData);
    }
    /* -- UnregisterEvent ---------------- Unregister a mouse click event -- */
    bool UnregisterEvent(LPARAM lEventId)
    {
      // Bail if invalid
      if(lEventId <= 0) return false;
      // Look for item
      LST_EVT::iterator itemEvent = listEvents.find(lEventId);
      // Not found? Bail
      if(itemEvent == listEvents.end()) return false;
      // Remove item
      listEvents.erase(itemEvent);
      // Done
      return true;
    }
    /* -- RegisterEvent -------------------- Register a mouse click event -- */
    bool RegisterEvent(LPARAM lParam, TC_CALLBACK cbFunc)
    {
      // Bail if invalid parameters
      if(lParam <= 0 || cbFunc == NULL) return false;
      // Look for item
      LST_EVT::iterator itemEvent = listEvents.find(lParam);
      // Found? Bail
      if(itemEvent != listEvents.end()) return false;
      // Create data structure
      TC_EVT sEvent;
      // Allocate structure
      sEvent.cbCallback = cbFunc;
      // Set message id
      sEvent.lParam = lParam;
      // Add to list
      listEvents[lParam] = sEvent;
      // Success
      return true;
    }
    /* -- Initialise ------------------------------------------------------- */
    void Initialise(LPCTSTR lpszResource, LPSTR szTip)
    {
      // Setup structure
      nidData.cbSize = sizeof(nidData);
      nidData.uID = 1;
      nidData.uFlags = NIF_MESSAGE;
      nidData.hWnd = programClass.windowClass.hwndWindow;
      nidData.uCallbackMessage = WM_APP + nidData.uID;
      // Get register window message event
      uiTbRestartEvent = RegisterWindowMessage(TEXT("TaskbarCreated"));
      // Set the icon
      SetIcon(lpszResource);
      // Set tip
      SetTip(szTip);
    }
    /* -- DeInitialise ----------------------------------------------------- */
    void DeInitialise(void)
    {
      // Remove icon
      Hide();
      // Unregister all mouse click events
      UnregisterAllEvents();
      // Reinit variables
      InitVariables();
    }
    /* -- InitVariables ---------------------------------------------------- */
    void InitVariables(void)
    {
      // Zero structure
      memset(&nidData, '\0', sizeof(nidData));
      // Reset restart event
      uiTbRestartEvent = 0;
    }
    /* -- Constructor ------------------------------------------------------ */
    TRAYICON(void) { InitVariables(); }
    /* -- Destructor ------------------------------------------------------- */
    ~TRAYICON(void) { DeInitialise(); }
    /* --------------------------------------------------------------------- */
  } trayIcon;
  /* ----------------------------------------------------------------------- */
  class MENU                         // Start of menu class
  { /* ----------------------------------------------------------- */ public:
    typedef struct _MENUITEM         // Start of menu item structure
    { /* ----------------------------------------------------------------- */
      UINT       uiParent;           // Parent menu id
      UINT       uiFlags;            // Menu flags
      HMENU      hMenu;              // Sub menu
      /* ----------------------------------------------------------------- */
    } MENUITEM;                      // End of menu item structure
    /* ------------------------------------------------------------------- */
    typedef std::map<UINT,MENUITEM> MENULIST; // Menu list
    /* ------------------------------------------------------------------- */
    MENULIST     menuData;           // Menu list
    HMENU        hmRootMenu;         // Root menu item
    const HMENU (CALLBACK MENU::*cbFunc)(void); // Create(Popup)Menu function
    /* ------------------------------------------------------------------- */
    inline const bool ItemExists(const size_t stId)
    {
      // Return if found
      return menuData.find(stId) != menuData.end();
    }
    /* ------------------------------------------------------------------- */
    inline MENULIST::iterator GetItem(const size_t stId)
    {
      // Return titem
      return menuData.find(stId);
    }
    /* ------------------------------------------------------------------- */
    const HMENU CALLBACK _CreatePopupMenu(void)
      { return CreatePopupMenu(); }
    /* ------------------------------------------------------------------- */
    const HMENU CALLBACK _CreateMenu(void)
      { return CreateMenu(); }
    /* ------------------------------------------------------------------- */
    const HMENU AddItem(const size_t _stId, const UINT _uiParent,
      const UINT _uiFlags, const LPCTSTR _lpszData)
    {
      // Bail if item exists
      if(ItemExists(_stId)) return NULL;
      // Find parent
      MENULIST::iterator menuItemClass = GetItem(_uiParent);
      // Bail if parent not found
      if(_uiParent > 0 && menuItemClass == menuData.end()) return NULL;
      // Create menu item structure
      MENUITEM menuItem = { _uiParent, _uiFlags, NULL };
      // Submenu required. Create it. Bail if failed
      if(_uiFlags & MF_POPUP && (menuItem.hMenu = (_stId == 0 ?
        (this->*cbFunc)() : CreatePopupMenu())) == NULL) return NULL;
      // Add to list
      menuData[_stId] = menuItem;
      // Was first item? Set the menu
      if(_stId == 0) return menuItem.hMenu;
      // Append menu item
      if(AppendMenu((*menuItemClass).second.hMenu, _uiFlags,
        _uiFlags & MF_POPUP ? (size_t)menuItem.hMenu : _stId, _lpszData) == FALSE)
        return NULL;
      // Success
      return menuItem.hMenu;
    }
    /* ------------------------------------------------------------------- */
    void SetPopupMenu(const bool bState)
    {
      // Set callback function
      cbFunc = bState == true ? &MENU::_CreatePopupMenu : &MENU::_CreateMenu;
    }
    /* ------------------------------------------------------------------- */
    const bool Initialise(const bool bPopupMenu)
    {
      // Deinitialise
      Deinitialise();
      // Set type of menu
      SetPopupMenu(bPopupMenu);
      // Add root item
      hmRootMenu = AddItem(0, 0, MF_POPUP, NULL);
      // Return status
      return hmRootMenu != NULL;
    }
    /* ------------------------------------------------------------------- */
    void Deinitialise(void)
    {
      // Walk menu array
      for(MENULIST::reverse_iterator menuItem = menuData.rbegin();
                                     menuItem != menuData.rend();
                                   ++menuItem)
        // Menu item has submenu
        if((*menuItem).second.uiFlags & MF_SEPARATOR)
          // Destroy the submenu
          DestroyMenu((*menuItem).second.hMenu);
      // Clear list
      menuData.clear();
      // Reinit variables
      InitVariables();
    }
    /* ------------------------------------------------------------------- */
    void InitVariables(void)
    {
      // Clear root item
      hmRootMenu = NULL;
      // Set global menu
      SetPopupMenu(false);
    }
    /* ------------------------------------------------------------------- */
    MENU(void)
    {
      // Reinit variables
      InitVariables();
    }
    /* ------------------------------------------------------------------- */
    ~MENU(void)
    {
      // Deinitialise
      Deinitialise();
    }
    /* ------------------------------------------------------------------- */
  };                                   // End of menu class
  /* == String Format Functions ============================================== */
  std::string FormatArguments(std::string strFormat, va_list vlArgs)
  {
    // Index
    size_t stIndex;
    // String stream which helps cleanly build the string
    std::ostringstream osStream;
    // Walk the string
    for(stIndex = 0; stIndex < strFormat.length(); ++stIndex)
      // Format delimiter?
      if(strFormat.at(stIndex) == '%')
        // Whats it's type?
        switch(strFormat.at(++stIndex))
        {
          // Null (Go back, don't want to cause overrun on next loop)
          case '\0': --stIndex; break;
          // Precision set
          case '.':
            // Compare next character
            switch(strFormat.at(++stIndex))
            {
              // Set floatfield to fixed
              case '+': osStream.setf(std::ios::fixed, std::ios::floatfield); break;
              // Set floatfield to off
              case '-': osStream.setf(0, std::ios::floatfield); break;
              // Number
              default: osStream.precision(strFormat.at(stIndex)-'0'); break;
            }
            // Done
            break;
          // Boolean
          case 'b': osStream << std::boolalpha << va_arg(vlArgs, bool); break;
          // Character
          case 'c': osStream << va_arg(vlArgs, char); break;
          // Double
          case 'd': osStream << va_arg(vlArgs, double); break;
          // Int
          case 'i': osStream << std::dec << va_arg(vlArgs, int); break;
          // Int (64-bit)
          case 'I': osStream << std::dec << va_arg(vlArgs, _int64); break;
          // Float
          case 'f': osStream << va_arg(vlArgs, float); break;
          // Pointer
          case 'p': osStream << va_arg(vlArgs, void*); break;
          // UTF-8 string
          case 's': osStream << va_arg(vlArgs, char*); break;
          // UTF-16 string
          case 'S':
          {
            // Create wstring to hold arg
            std::wstring wsString = va_arg(vlArgs, wchar_t*);
            // Create string
            std::string sString;
            sString.reserve(wsString.length());
            std::transform(wsString.begin(), wsString.end(),
              std::back_inserter(sString), [](wchar_t c){ return (char)c; });
            // Send to stream
            osStream << sString;
            // Done
            break;
          }
          // Thousand separated number
//          case 't': osStream << FormatNumber(va_arg(vlArgs, int)).c_str(); break;
          // Thousand separated 64-bit number
//          case 'T': osStream << FormatNumber(va_arg(vlArgs, _int64)).c_str(); break;
          // Hexadecimal
          case 'x': osStream << std::hex << va_arg(vlArgs, int); break;
          // Hexadecimal (64-Bit)
          case 'X': osStream << std::hex << va_arg(vlArgs, _int64); break;
          // Percent symbol
          case '%': osStream << '%'; break;
          // Not supported
          default: osStream << "?"; break;
        }
      // Normal character
      else osStream << strFormat.at(stIndex);
    // Done return string
    return osStream.str();
  }
  /* ------------------------------------------------------ Format a string -- */
  std::string FormatString(std::string strFormat, ...)
  {
    // Create pointer to arguments
    va_list vlArgs;
    // Get pointer to arguments list (...)
    va_start(vlArgs, strFormat);
    // Format arguments list
    strFormat = FormatArguments(strFormat, vlArgs);
    // Done with arguments list
    va_end(vlArgs);
    // Return result
    return strFormat;
  }
  /* ----------------------------------------------------------------------- */
  void Error(const int iCode, std::string strFormat, ...)
  {
    // Create pointer to arguments
    va_list vlArgs;
    // Get pointer to arguments
    va_start(vlArgs, strFormat);
    // Build string
    strFormat = programClass.FormatArguments(strFormat, vlArgs);
    // Show error
    MessageBox(windowClass.hwndWindow, strFormat.c_str(), NULL,
      iCode < 0 ? MB_ICONEXCLAMATION :
      (iCode > 0 ? MB_ICONSTOP : MB_ICONINFORMATION));
    // Clean up arguments
    va_end(vlArgs);
    // Bail
    if(iCode > 0) exit(iCode);
  }
  /* ----------------------------------------------------------------------- */
  template<typename T>std::string BytesToReadable(const T tSize)
  {
    // Decide which part to display
    if(tSize < 1024)
      return FormatString("%iB", (int)tSize);
    else if(tSize < 1048576)
      return FormatString("%.+%.2%dkB", (double)tSize / 1024);
    else if(tSize < 1073741824)
      return FormatString("%.+%.2%dmB", (double)tSize / 1048576);
    return FormatString("%.+%.2%dgB", (double)tSize / 1073741824);
  }
  /* ----------------------------------------------------------------------- */
  static const bool CALLBACK OnActivateApp(const WPARAM wP, const LPARAM)
  {
    // Hide window if requested
    if(wP == false) programClass.windowClass.Show(SW_HIDE);
    // Event handled
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static BOOL CALLBACK RefreshWindowCallback(const HWND hWnd, PROGRAM &programClass)
  {
    // Bail if not window
    if(hWnd == NULL || !IsWindow(hWnd)) return TRUE;
    // Get window pid
    DWORD dwPid;
    const DWORD dwTid = GetWindowThreadProcessId(hWnd, &dwPid);
    // Ignore if this program owns the window
    if(dwTid == GetCurrentThreadId()) return TRUE;
    // Get text
    const int iLength = GetWindowTextLength(hWnd);
    // Ignore if no text
    if(iLength <= 0) return TRUE;
    // Get window styles
    const LONG_PTR dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    const LONG_PTR dwExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    // Ignore if not have popup status
//    if(~dwStyle & WS_CAPTION) return TRUE;
    // Allocate
    char *cpText = new char[iLength+1];
    // Store
    GetWindowText(hWnd, cpText, iLength+1);
    // Get window class name
    char caClassName[256];
    GetClassName(hWnd, caClassName, sizeof(caClassName));
    // Add to list
    programClass.InsertItem(cpText, caClassName,
      programClass.FormatString("%i", dwTid),
      programClass.FormatString("%i", dwPid),
      programClass.FormatString("0x%x", dwStyle),
      programClass.FormatString("0x%x", dwExStyle),
      programClass.FormatString("%i", hWnd));
    // Free
    delete []cpText;
    // Continue refreshing
    return TRUE;
  }
  /* ----------------------------------------------------------------------- */
  void RefreshList(void)
  {
    // Clear list
    windowClass.GetControl(1).SendMessage(LVM_DELETEALLITEMS, 0, 0);
    // Enumerate
    EnumWindows((WNDENUMPROC)RefreshWindowCallback, (LPARAM)this);
  }
  /* ----------------------------------------------------------------------- */
  void InsertItem(std::string strLabel, std::string strClassName,
   std::string strTid, std::string strPid,
    std::string strStyle, std::string strExStyle, std::string strWindow)
  {
    // Set data
    LVITEM lviData = { 0 };
    lviData.mask = LVIF_TEXT;
    // Set main column
    lviData.pszText = (LPSTR)strLabel.c_str();
    lviData.cchTextMax = (int)strLabel.length();
    lviData.iItem = (int)windowClass.GetControl(1).
      SendMessage(LVM_INSERTITEM, 0, (LPARAM)&lviData);
    lviData.iSubItem = 1;
    lviData.pszText = (LPSTR)strClassName.c_str();
    lviData.cchTextMax = (int)strClassName.length();
    windowClass.GetControl(1).SendMessage(LVM_SETITEM, 0, (LPARAM)&lviData);
    lviData.iSubItem = 2;
    lviData.pszText = (LPSTR)strTid.c_str();
    lviData.cchTextMax = (int)strTid.length();
    windowClass.GetControl(1).SendMessage(LVM_SETITEM, 0, (LPARAM)&lviData);
    lviData.iSubItem = 3;
    lviData.pszText = (LPSTR)strPid.c_str();
    lviData.cchTextMax = (int)strPid.length();
    windowClass.GetControl(1).SendMessage(LVM_SETITEM, 0, (LPARAM)&lviData);
    lviData.iSubItem = 4;
    lviData.pszText = (LPSTR)strStyle.c_str();
    lviData.cchTextMax = (int)strStyle.length();
    windowClass.GetControl(1).SendMessage(LVM_SETITEM, 0, (LPARAM)&lviData);
    lviData.iSubItem = 5;
    lviData.pszText = (LPSTR)strExStyle.c_str();
    lviData.cchTextMax = (int)strExStyle.length();
    windowClass.GetControl(1).SendMessage(LVM_SETITEM, 0, (LPARAM)&lviData);
    lviData.iSubItem = 6;
    lviData.pszText = (LPSTR)strWindow.c_str();
    lviData.cchTextMax = (int)strWindow.length();
    windowClass.GetControl(1).SendMessage(LVM_SETITEM, 0, (LPARAM)&lviData);
  }
  /* ----------------------------------------------------------------------- */
  void InsertColumn(const int iIndex, const int iSize, std::string strLabel, const int iFmt)
  {
    // Set column data
    LVCOLUMN lvcData = { 0 };
    lvcData.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
    lvcData.fmt = iFmt;
    lvcData.cx = iSize;
    lvcData.pszText = (LPSTR)strLabel.c_str();
    lvcData.cchTextMax = (int)strLabel.length();
    // Create
    windowClass.GetControl(1).SendMessage(LVM_INSERTCOLUMN, (WPARAM)iIndex, (LPARAM)&lvcData);
  }
  /* ----------------------------------------------------------------------- */
  void OpenMenu(const LPNMITEMACTIVATE nmiaData)
  {
    // Get handle of item
    LVITEM lviData = { 0 };
    char caData[16];
    lviData.pszText = caData;
    lviData.cchTextMax = sizeof(caData);
    lviData.iSubItem = 6;
    windowClass.GetControl(1).SendMessage(LVM_GETITEMTEXT, nmiaData->iItem, (LPARAM)&lviData);
    // Transfer the number to HWND
    HWND hwndWindow = (HWND)atoll(lviData.pszText);
    // Handle is invalid?
    if(hwndWindow == NULL || !IsWindow(hwndWindow))
    {
      // Refresh list
      RefreshList();
      // Halt execution of menu
      return;
    }
    // Get window style
    const LONG_PTR dwStyle = GetWindowLongPtr(hwndWindow, GWL_STYLE);
    const LONG_PTR dwExStyle = GetWindowLongPtr(hwndWindow, GWL_EXSTYLE);
    // Create menu class and initialise it
    MENU menuClass;
    if(menuClass.Initialise(true) == false)
      Error(1, "Menu build failed!");
    menuClass.AddItem(1, 0, MF_STRING|MF_DISABLED, lviData.pszText);
    menuClass.AddItem(2, 0, MF_SEPARATOR, NULL);
    menuClass.AddItem(3, 0, MF_STRING|(dwStyle&WS_ICONIC?MF_DISABLED:0), "&Minimize");
    menuClass.AddItem(4, 0, MF_STRING|(dwStyle&WS_MAXIMIZE?MF_DISABLED:0), "M&aximize");
    menuClass.AddItem(5, 0, MF_STRING|(dwStyle&WS_ICONIC?0:MF_DISABLED), "&Restore");
    menuClass.AddItem(6, 0, MF_STRING, "&Close");
    menuClass.AddItem(7, 0, MF_STRING, "&Destroy");
    menuClass.AddItem(8, 0, MF_SEPARATOR, NULL);
    menuClass.AddItem(9, 0, MF_STRING|(dwStyle&WS_VISIBLE?MF_CHECKED:0), "&Visible");
    menuClass.AddItem(10, 0, MF_STRING|(dwExStyle&WS_EX_TOPMOST?MF_CHECKED:0), "&Lock");
    // Show popup menu
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    // Show menu and grab result
    switch(TrackPopupMenu(menuClass.hmRootMenu,
      TPM_RETURNCMD|TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_LEFTBUTTON,
      cursorPos.x, cursorPos.y, 0, windowClass.hwndWindow, NULL))
    {
      // Minimize window
      case 3: ShowWindow(hwndWindow, SW_SHOWMINIMIZED); break;
      // Maximize window
      case 4: ShowWindow(hwndWindow, SW_SHOWMAXIMIZED); break;
      // Restore window
      case 5: ShowWindow(hwndWindow, SW_RESTORE); break;
      // Close window
      case 6: CloseWindow(hwndWindow); break;
      // Destroy window
      case 7: DestroyWindow(hwndWindow); break;
      // Show or hide window
      case 9: ShowWindow(hwndWindow, dwStyle&WS_VISIBLE?SW_HIDE:SW_SHOWNORMAL); break;
      // Make window always on top or not
      case 10: SetWindowPos(hwndWindow, dwExStyle&WS_EX_TOPMOST?HWND_NOTOPMOST:HWND_TOPMOST, -1, -1, -1, -1, SWP_NOMOVE|SWP_NOREPOSITION|SWP_NOSIZE); break;
      // Unsupported menu value
      default: return;
    }
  }
  /* ----------------------------------------------------------------------- */
  static const bool CALLBACK OnNotify(const WPARAM wP, const LPARAM lP)
  {
    // Not handled
    if(wP != 1) return false;
    // What code?
    switch(((LPNMHDR)lP)->code)
    {
      // Right click
      case NM_RCLICK: programClass.OpenMenu((LPNMITEMACTIVATE)lP); break;
      // Unhandled
      default:        return false;
    }
    // Handled
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static const bool CALLBACK OnCreate(const WPARAM, const LPARAM)
  {
    // Init size and position
    programClass.windowClass.Resize(272, 254);
    programClass.windowClass.MoveEnd(0, 0);
    // Set font
    programClass.windowClass.SetFont("Tahoma", 13);
    // Initialise common controls
    INITCOMMONCONTROLSEX ccData = { sizeof(ccData), ICC_LISTVIEW_CLASSES };
    if(InitCommonControlsEx(&ccData) == false)
      programClass.Error(1, "Failed to initialise common control classes!");
    // Register classes
    programClass.windowClass.AddControl( 1,   8,   8, 256, 238, WC_LISTVIEW,     "",                 WS_EX_CLIENTEDGE, WS_VISIBLE|WS_TABSTOP|WS_HSCROLL|WS_HSCROLL|WS_VSCROLL|LVS_REPORT|LVS_SORTASCENDING);
    programClass.windowClass.GetControl(1).
      SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
    programClass.InsertColumn(0, 235, "Title",   LVCFMT_LEFT);
    programClass.InsertColumn(1, 235, "Class",   LVCFMT_LEFT);
    programClass.InsertColumn(2, 48,  "Tid",     LVCFMT_RIGHT);
    programClass.InsertColumn(3, 48,  "Pid",     LVCFMT_RIGHT);
    programClass.InsertColumn(4, 72,  "Style",   LVCFMT_RIGHT);
    programClass.InsertColumn(5, 72,  "ExStyle", LVCFMT_RIGHT);
    programClass.InsertColumn(6, 64,  "Handle",  LVCFMT_RIGHT);
    // Done
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static const bool CALLBACK OnTrayIconClick(void)
  {
    // Show window
    programClass.windowClass.Show(SW_SHOW);
    // Activate
    programClass.windowClass.Activate();
    // Refresh list
    programClass.RefreshList();
    // Set focus on list
    programClass.windowClass.GetControl(1).Focus();
    // Don't block
    return false;
  }
  /* ----------------------------------------------------------------------- */
  PROGRAM(void)
  {
    // Handle window creation
    windowClass.AddEvent(WM_CREATE, OnCreate);
    windowClass.AddEvent(WM_ACTIVATEAPP, OnActivateApp);
    windowClass.AddEvent(WM_NOTIFY, OnNotify);
    // Initialise, resize and reposition window
    windowClass.Initialise("Window Manager", 1,
      WS_EX_TOOLWINDOW|WS_EX_TOPMOST, WS_POPUP|WS_CAPTION|WS_SYSMENU);
    // Initialise tray icon
    trayIcon.Initialise(MAKEINTRESOURCE(1), "Window Manager");
    trayIcon.RegisterEvent(WM_LBUTTONUP, OnTrayIconClick);
    // Show tray icon
    trayIcon.Show();
    // Go
    windowClass.Main();
  }
  /* ----------------------------------------------------------------------- */
} programClass;                        // Program class
/* ========================================================================= */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT) { return 0; }
/* ========================================================================= */
