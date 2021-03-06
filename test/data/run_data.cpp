
#include "run_data.h"
#include "test_btrfs.h"
#include "test_device.h"
#include "test_mtab.h"
#include "test_devicelist.h"
#include "test_deviceinfo.h"

class TDataRunner:public TRunner
{
protected:
		
	virtual void doRun()
	{
		runner(new TTestBtrfs());
		runner(new TTestDevice());
		runner(new TTestDataMTab());
		runner(new TTestDataDeviceList());
		runner(new TTestDataDeviceInfo());
	}
	
public:
	TDataRunner(TTestConfig *p_testConfig):TRunner(p_testConfig)
	{
		
	}

};

bool run_data(TTestConfig* p_config)
{

	TDataRunner l_run(p_config);
	return l_run.run();
}


