#pragma warning(disable: 4996)
#include <stdexcept>
#include "error.h"

namespace Reg {
	class CreateKey {
		HKEY hkey;
		DWORD disp;
	public:
		CreateKey(HKEY hKey, PCTSTR lpSubKey)
		{
			LSTATUS status = RegCreateKeyEx(hKey, lpSubKey, 0, 0, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, 0, &hkey, &disp);
			if (status != ERROR_SUCCESS) {
				throw std::runtime_error("CreateKey: RegCreateKeyEx failed");
			}
		}
		CreateKey(const CreateKey&) = delete;
		CreateKey& operator=(const CreateKey&) = delete;
		~CreateKey()
		{
			RegCloseKey(hkey);
		}
		operator HKEY() const
		{
			return hkey;
		}
		DWORD disposition() const
		{
			return disp;
		}
		struct Proxy {
			CreateKey& key;
			PCTSTR value;
			Proxy(CreateKey& key, PCTSTR value)
				: key(key), value(value)
			{ }
			CreateKey& operator=(DWORD dword)
			{
				LSTATUS status = RegSetValueEx(key, value, 0, REG_DWORD, (const BYTE*)&dword, sizeof(DWORD));
				if (ERROR_SUCCESS != status)
				{
					throw std::runtime_error("RegSetValueEx failed");
				}

				return key;
			}
			// operator=(string) etc
		};
		Proxy operator[](PCTSTR value)
		{
			return Proxy(*this, value);
		}
	};
}

class reg_alert_level {
	DWORD value;
public:
	reg_alert_level()
		: value(0x7)
	{
		DWORD size = sizeof(DWORD);
		LSTATUS status = RegGetValue(
			HKEY_CURRENT_USER, 
			TEXT("Software\\KALX\\xll"),
			TEXT("xll_alert_level"), 
			RRF_RT_REG_DWORD, 0,
			&value, &size);
		if (ERROR_SUCCESS != status) 
		{
			operator=(value);
		}
	}
	reg_alert_level& operator=(DWORD level)
	{
		Reg::CreateKey key(HKEY_CURRENT_USER, TEXT("Software\\KALX\\xll"));
		key[TEXT("xll_alert_level")] = level;

		return *this;
	}
	operator DWORD() const
	{
		return value;
	}
} xll_alert_level;

DWORD XLL_ALERT_LEVEL(DWORD level)
{
    DWORD olevel = xll_alert_level;
    
    xll_alert_level = level;

    return olevel;
}

int 
XLL_ALERT(const char* text, const char* caption, DWORD level, UINT type, bool force)
{
	try {
		if ((xll_alert_level&level) || force) {
			if (IDCANCEL == MessageBoxA(GetForegroundWindow(), text, caption, MB_OKCANCEL|type))
				xll_alert_level = (xll_alert_level & ~level);
		}
	}
	catch (const std::exception& ex) {
		MessageBoxA(GetForegroundWindow(), ex.what(), "Alert", MB_OKCANCEL| MB_ICONERROR);
	}

	return static_cast<int>(xll_alert_level);
}

int 
XLL_ERROR(const char* e, bool force)
{
	return XLL_ALERT(e, "Error", XLL_ALERT_ERROR, MB_ICONERROR, force);
}
int 
XLL_WARNING(const char* e, bool force)
{
	return XLL_ALERT(e, "Warning", XLL_ALERT_WARNING, MB_ICONWARNING, force);
}
int 
XLL_INFO(const char* e, bool force)
{
	return XLL_ALERT(e, "Information", XLL_ALERT_INFO, MB_ICONINFORMATION, force);
}

#ifdef _DEBUG
#if 0
struct test_registry {
    Reg::Key key;
	test_registry()
	{
		key = Reg::Key(HKEY_CURRENT_USER, TEXT("tmp\\key"));
        LSTATUS status;
        status = RegDeleteKey(key, TEXT("tmp\\key"));
        using tstring = std::basic_string<TCHAR>;
        Reg::Key key2(HKEY_CURRENT_USER, TEXT("tmp\\string"));
        Reg::Entry<tstring> value(key2, TEXT("value"));
        tstring s = value;
        value = TEXT("text");
        s = value;
        s = s;
    }
	~test_registry()
	{
	}
};
test_registry _{};
#endif // 0
#endif // _DEBUG