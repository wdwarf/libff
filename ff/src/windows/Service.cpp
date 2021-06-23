#include <ff/windows/Service.h>
#include <windows.h>
#include <winsvc.h>
#include <ff/String.h>
#include <ff/Application.h>

using namespace std;

NS_FF_BEG

Service* Service::serviceObject = NULL;

static string GetErrorMessage(DWORD errCode)
{
	string msg;
	LPSTR lpMsgBuf = 0;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		0,
		errCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		NULL);
	LocalFree(lpMsgBuf);
	msg = lpMsgBuf;
	return msg;
}

Service::Service(void)
{
	::InitializeCriticalSection(&this->cs);
	this->terminated = false;
	Service::serviceObject = this;
	this->desiredAccess = SC_MANAGER_ALL_ACCESS;
	this->serviceType = SERVICE_WIN32_OWN_PROCESS;
	this->startType = SERVICE_AUTO_START;
	this->hEvt = CreateEvent(0, TRUE, FALSE, NULL);
}

Service::Service(const std::string& serviceName, const std::string& displayName)
{
	this->setServiceName(serviceName);
	this->setDisplayName(displayName);

	::InitializeCriticalSection(&this->cs);
	this->terminated = false;
	Service::serviceObject = this;
	this->desiredAccess = SC_MANAGER_ALL_ACCESS;
	this->serviceType = SERVICE_WIN32_OWN_PROCESS;
	this->startType = SERVICE_AUTO_START;
	this->hEvt = CreateEvent(0, TRUE, FALSE, NULL);
}

Service::~Service(void)
{
	::DeleteCriticalSection(&this->cs);
	::CloseHandle(this->hEvt);
}

void Service::lock()
{
	::EnterCriticalSection(&this->cs);
}

void Service::unlock()
{
	::LeaveCriticalSection(&this->cs);
}

int Service::getArgc() const
{
	return this->m_argc;
}

char **Service::getArgv() const
{
	return this->m_argv;
}

string Service::getServiceName()
{
	return this->serviceName;
}

void Service::setServiceName(const string& serviceName)
{
	this->serviceName = serviceName;
	if (this->displayName.empty())
	{
		this->displayName = serviceName;
	}
}

void Service::setDisplayName(const string& displayName)
{
	this->displayName = displayName;
}

bool Service::isTerminated()
{
	return this->terminated;
}

LPSERVICE_MAIN_FUNCTIONA Service::getServiceMainFunc()
{
	return Service::ServiceMain;
}

Service* Service::GetServiceObject()
{
	return Service::serviceObject;
}

void WINAPI Service::ServiceMain(DWORD argc, LPSTR* argv)
{
	Service* obj = Service::GetServiceObject();
	if (!obj)
	{
		return;
	}

	obj->terminated = false;
	obj->exited = false;

	obj->ServiceStatus.dwServiceType = SERVICE_WIN32;
	obj->ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	obj->ServiceStatus.dwControlsAccepted =
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	obj->ServiceStatus.dwWin32ExitCode = 0;
	obj->ServiceStatus.dwServiceSpecificExitCode = 0;
	obj->ServiceStatus.dwCheckPoint = 0;
	obj->ServiceStatus.dwWaitHint = 0;

	obj->hStatus = RegisterServiceCtrlHandlerA(
		obj->serviceName.c_str(),
		(LPHANDLER_FUNCTION)Service::ControlHandler);

	if (obj->hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		obj->exited = true;
		return;
	}

	obj->ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	if (!SetServiceStatus(obj->hStatus, &obj->ServiceStatus))
	{
		obj->exited = true;
		return;
	}

	try
	{
		obj->onRun();
	}
	catch (...)
	{
	}

	obj->lock();
	obj->exited = true;
	obj->ServiceStatus.dwWin32ExitCode = 0;
	obj->ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(obj->hStatus, &obj->ServiceStatus);
	Service::serviceObject = 0;
	obj->unlock();

	return;
}

void Service::ControlHandler(DWORD request)
{
	Service* obj = Service::GetServiceObject();
	if (obj)
	{
		switch (request)
		{

		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
		{
			obj->lock();
			obj->ServiceStatus.dwWin32ExitCode = 0;
			obj->ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(obj->hStatus, &obj->ServiceStatus);
			obj->terminated = true;
			obj->serviceEnd();
			obj->unlock();
			return;
		}
		default:
			break;
		}
		//SetServiceStatus(obj->hStatus, &obj->ServiceStatus);
	}
}

int Service::installService()
{
	int re = -1;
	if (!this->serviceName.empty())
	{
		if (this->displayName.empty())
		{
			this->displayName = this->serviceName;
		}
		string binPath = Application::GetApplicationPath();
		if (!this->param.empty())
		{
			binPath = "\"" + binPath + "\" " + this->param;
		}
		//TCHAR binPath[256] = {0};
		//::GetModuleFileName(NULL, binPath, 256);
		SC_HANDLE hSCManager = ::OpenSCManagerA(
			0,
			0,
			SC_MANAGER_CREATE_SERVICE);
		if (nullptr == hSCManager)
		{
			cout << GetErrorMessage(::GetLastError()) << endl;
			return re;
		}

		int dBufLen = 0;
		for (auto& dep : dependencies)
		{
			dBufLen += dep.length() + 1;
		}
		dBufLen += 2;
		vector<char> dBuf(dBufLen);
		ZeroMemory(&dBuf[0], dBufLen);
		int len = 0;
		for (auto& dep : dependencies)
		{
			CopyMemory(&dBuf[0] + len, dep.c_str(), dep.length());
			len += dep.length() + 1;
		}

		SC_HANDLE hService = ::CreateServiceA(
			hSCManager,
			this->serviceName.c_str(),
			this->displayName.c_str(),
			this->desiredAccess,
			this->serviceType,
			this->startType,
			SERVICE_ERROR_IGNORE,
			binPath.c_str(),
			0,
			0,
			&dBuf[0],
			(this->startName.empty() ? 0 : this->startName.c_str()),
			(this->password.empty() ? 0 : this->password.c_str()));
		stringstream infoStr;
		infoStr << "Service " << this->serviceName << " install "
			<< (nullptr != hService ? "success." : ("failed, " + GetErrorMessage(::GetLastError())));

		if (nullptr != hService)
			::CloseServiceHandle(hService);
		if (nullptr != hSCManager)
			::CloseServiceHandle(hSCManager);

		cout << infoStr.str() << endl;
	}
	return re;
}

int Service::uninstallService()
{
	if (this->serviceName.empty())
		return -1;

	SC_HANDLE hSCManager = ::OpenSCManagerA(
		0,
		0,
		SC_MANAGER_CREATE_SERVICE);
	if (nullptr == hSCManager)
	{
		cout << GetErrorMessage(::GetLastError()) << endl;
		return -1;
	}

	SC_HANDLE
		hService = ::OpenServiceA(
			hSCManager,
			this->serviceName.c_str(),
			this->desiredAccess);

	int re = ((nullptr != hService) && (TRUE == ::DeleteService(hService))) ? 0 : -1;
	stringstream infoStr;
	infoStr << "Service " << this->serviceName << " uninstall "
		<< (0 == re ? "success." : ("failed, " + GetErrorMessage(::GetLastError())));

	if (nullptr != hService)
		::CloseServiceHandle(hService);
	if (nullptr != hSCManager)
		::CloseServiceHandle(hSCManager);

	cout << infoStr.str() << endl;

	return re;
}

int Service::Run(int argc, char* argv[])
{
	Service* obj = Service::GetServiceObject();
	if (nullptr == obj)
		return -1;

	obj->m_argc = argc;
	obj->m_argv = argv;

	for (int i = 1; i < argc; ++i)
	{
		obj->params.push_back(argv[i]);
	}

	obj->onInitialize();
	string serviceName = obj->getServiceName();

	if (argc > 1)
	{
		string arg = argv[1];
		int pos = 0;
		ToLower(Trim(arg));
		pos = ((pos = arg.find("/")) < 0) ? arg.find("-") : pos;
		if (0 == pos)
		{
			arg = arg.substr(1, arg.length() - 1);
		}

		if ("install" == arg)
		{
			if (argc > 2)
			{
				obj->param = argv[2];
			}
			obj->beforeInstall();
			int re = obj->installService();
			obj->afterInstall();
			return re;
		}

		if ("uninstall" == arg)
		{
			obj->beforeUninstall();
			int re = obj->uninstallService();
			obj->afterUninstall();
			return re;
		}
	}

	SERVICE_TABLE_ENTRYA Service_Table[] =
	{
		{(char*)serviceName.c_str(), Service::ServiceMain},
		{NULL, NULL}
	};

	StartServiceCtrlDispatcherA(Service_Table);

	return 0;
}

void Service::waitForExecute()
{
	::WaitForSingleObject(this->hEvt, INFINITE);
}

void Service::serviceEnd()
{
	::SetEvent(this->hEvt);
}

NS_FF_END
