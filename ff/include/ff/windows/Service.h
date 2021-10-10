#ifndef FF_WINDOWS_SERVICE_H_
#define FF_WINDOWS_SERVICE_H_

#include <ff/ff_config.h>
#include <functional>
#include <string>
#include <vector>

NS_FF_BEG

class LIBFF_API Service
{
public:
	Service(void);
	Service(const std::string& serviceName, const std::string& displayName = "");
	virtual ~Service(void);

	virtual void onRun() = 0;

	int getArgc() const;
	char** getArgv() const;
	std::string getServiceName();
	void setServiceName(const std::string& serviceName);
	void setDisplayName(const std::string& displayName);
	void serviceEnd();

	static LPSERVICE_MAIN_FUNCTIONA getServiceMainFunc();
	static Service* GetServiceObject();

	static int Run(int argc, char* argv[]);

protected:
	std::string serviceName;
	std::string displayName;
	int m_argc;
	char** m_argv;
	DWORD desiredAccess;
	DWORD serviceType;
	DWORD startType;
	std::string startName;
	std::string password;
	std::string param;
	std::vector<std::string> dependencies;

	virtual void onInitialize() {};

	bool isTerminated();
	int installService();
	int uninstallService();
	virtual void beforeInstall() {};
	virtual void afterInstall() {};
	virtual void beforeUninstall() {};
	virtual void afterUninstall() {};

	std::vector<std::string> params;

protected:
	void waitForExecute();

private:
	bool terminated;
	bool exited;

	SERVICE_STATUS ServiceStatus;
	SERVICE_STATUS_HANDLE hStatus;

	CRITICAL_SECTION cs;
	void lock();
	void unlock();

	HANDLE hEvt;

	static void WINAPI ServiceMain(DWORD argc, LPSTR* argv);
	static void ControlHandler(DWORD request);
	static Service* serviceObject;
};

NS_FF_END

#endif
