#include "testusertable.h"
#include "testbooktable.h"
#include "teststudenttable.h"
#include "testlendingtable.h"

// Executes all tests
// Note: This is equivalent to QTEST_APPLESS_MAIN for multiple test classes.
int main(int argc, char** argv)
{
   int status = 0;
   {
      TestUserTable tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      TestBookTable tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      TestStudentTable tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
       TestLendingTable tc;
       status |= QTest::qExec(&tc, argc, argv);
   }
   return status;
}
