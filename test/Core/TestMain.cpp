#include "gtest/gtest.h"
using namespace std;

int main(int argc, char** argv) 
{
#if 1
	//Run a single test of Alg Part
	::testing::GTEST_FLAG(filter) = "CPU_Precison.test1";
#else
	//Run All Core Part
#endif

	::testing::InitGoogleTest(&argc, argv);
	const auto ret = RUN_ALL_TESTS();
	cout << "Core Part GTest over, press Enter to continue." << endl;
	getchar();
	return ret;
}