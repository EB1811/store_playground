#include "Misc/AutomationTest.h"
#include "GlobalDataManager.h"

#define TEST_TRUE(expression) EPIC_TEST_BOOLEAN_(TEXT(#expression), expression, true)
#define TEST_FALSE(expression) EPIC_TEST_BOOLEAN_(TEXT(#expression), expression, false)
#define TEST_EQUAL(expression, expected) EPIC_TEST_BOOLEAN_(TEXT(#expression), expression, expected)
#define EPIC_TEST_BOOLEAN_(text, expression, expected) TestEqual(text, expression, expected);

#define MONEY(value)                    \
  {                                     \
    { EReqFilterOperand::Money, value } \
  }

#define ARRAY_NAMES(...)                                                \
  {                                                                     \
    {                                                                   \
      EReqFilterOperand::QuestsCompleted, TArray<FName> { __VA_ARGS__ } \
    }                                                                   \
  }

BEGIN_DEFINE_SPEC(AGlobalDataManagerSpec,
                  "store_playground.GlobalDataManager",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
TSharedPtr<AGlobalDataManager> GlobalDataManager;
END_DEFINE_SPEC(AGlobalDataManagerSpec)
void AGlobalDataManagerSpec::Define() {
  Describe("EvaluateRequirementsFilter basic operators.", [this]() {
    It("should return true if the filter is empty", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(NAME_None, MONEY(0)));
      TEST_TRUE(EvaluateRequirementsFilter("", MONEY(0)));
    });

    It("should evaluate simple money equality correctly", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money = 100"), MONEY(100)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money = 100"), MONEY(50)));
    });

    It("should evaluate money inequality correctly", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money != 100"), MONEY(50)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money != 100"), MONEY(100)));
    });

    It("should evaluate money less than correctly", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money < 100"), MONEY(50)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money < 100"), MONEY(100)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money < 100"), MONEY(150)));
    });

    It("should evaluate money less than or equal correctly", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money <= 100"), MONEY(50)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money <= 100"), MONEY(100)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money <= 100"), MONEY(150)));
    });

    It("should evaluate money greater than correctly", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money > 100"), MONEY(150)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money > 100"), MONEY(100)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money > 100"), MONEY(50)));
    });

    It("should evaluate money greater than or equal correctly", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100"), MONEY(150)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100"), MONEY(100)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money >= 100"), MONEY(50)));
    });

    It("should evaluate AND logical operations correctly", EAsyncExecution::ThreadPool, [this]() {
      // Both conditions true
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 200"), MONEY(150)));

      // First condition true, second false
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 200"), MONEY(250)));

      // First condition false, second true
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 200"), MONEY(50)));

      // Both conditions false
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 200"), MONEY(0)));
    });

    It("should evaluate OR logical operations correctly", EAsyncExecution::ThreadPool, [this]() {
      // Both conditions true
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100 OR Money <= 50"), MONEY(150)));

      // First condition true, second false
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100 OR Money <= 50"), MONEY(150)));

      // First condition false, second true
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100 OR Money <= 50"), MONEY(25)));

      // Both conditions false
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money >= 100 OR Money <= 50"), MONEY(75)));
    });

    It("should evaluate complex expressions correctly", EAsyncExecution::ThreadPool, [this]() {
      // Multiple ANDs
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 200 AND Money != 150"), MONEY(175)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 200 AND Money != 150"), MONEY(150)));

      // Multiple ORs
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money <= 50 OR Money >= 200 OR Money = 125"), MONEY(125)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money <= 50 OR Money >= 200 OR Money = 125"), MONEY(25)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money <= 50 OR Money >= 200 OR Money = 125"), MONEY(100)));

      // Mixed AND and OR (evaluates left to right)
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 300 OR Money = 50"), MONEY(200)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 300 OR Money = 50"), MONEY(50)));
      TEST_FALSE(EvaluateRequirementsFilter(FName("Money >= 100 AND Money <= 300 OR Money = 50"), MONEY(75)));
    });

    It("should handle whitespace variations correctly", EAsyncExecution::ThreadPool, [this]() {
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money=100"), MONEY(100)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money = 100"), MONEY(100)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money  =  100"), MONEY(100)));
    });

    It("should handle edge cases correctly", EAsyncExecution::ThreadPool, [this]() {
      // Zero values
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money = 0"), MONEY(0)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money >= 0"), MONEY(100)));

      // Negative values (assuming they're supported)
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money = -100"), MONEY(-100)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money > -100"), MONEY(0)));

      // Nearly equal values
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money = 100"), MONEY(100.0001f)));
      TEST_TRUE(EvaluateRequirementsFilter(FName("Money = 100"), MONEY(99.9999f)));
    });

    It("should evaluate array contains operator correctly", EAsyncExecution::ThreadPool, [this]() {
      // Single item
      TEST_TRUE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, Quest1)"), ARRAY_NAMES(FName("Quest1"))));
      TEST_FALSE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, Quest2)"), ARRAY_NAMES(FName("Quest1"))));

      // Multiple items - array contains all specified items
      TEST_TRUE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, [Quest1, Quest2])"),
                                           ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));
      TEST_FALSE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, [Quest1, Quest4])"),
                                            ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));

      // Empty array
      TEST_FALSE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, Quest1)"), ARRAY_NAMES()));

      // Array with many items
      TEST_TRUE(EvaluateRequirementsFilter(
          FName("contains(QuestsCompleted, Quest5)"),
          ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"), FName("Quest4"), FName("Quest5"))));
    });

    It("should evaluate array count comparisons correctly", EAsyncExecution::ThreadPool, [this]() {
      // Count equals
      TEST_TRUE(EvaluateRequirementsFilter(FName("QuestsCompleted = 3"),
                                           ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));
      TEST_FALSE(EvaluateRequirementsFilter(FName("QuestsCompleted = 2"),
                                            ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));

      // Count greater than
      TEST_TRUE(EvaluateRequirementsFilter(FName("QuestsCompleted > 2"),
                                           ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));
      TEST_FALSE(EvaluateRequirementsFilter(FName("QuestsCompleted > 3"),
                                            ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));

      // Count less than
      TEST_TRUE(EvaluateRequirementsFilter(FName("QuestsCompleted < 4"),
                                           ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));
      TEST_FALSE(EvaluateRequirementsFilter(FName("QuestsCompleted < 3"),
                                            ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));
    });

    It("should evaluate complex expressions with arrays correctly", EAsyncExecution::ThreadPool, [this]() {
      // Array contains with money condition
      TEST_TRUE(EvaluateRequirementsFilter(
          FName("contains(QuestsCompleted, Quest1) AND Money >= 100"),
          {{EReqFilterOperand::QuestsCompleted, TArray<FName>{FName("Quest1")}}, {EReqFilterOperand::Money, 100.0f}}));

      TEST_FALSE(EvaluateRequirementsFilter(
          FName("contains(QuestsCompleted, Quest1) AND Money >= 100"),
          {{EReqFilterOperand::QuestsCompleted, TArray<FName>{FName("Quest1")}}, {EReqFilterOperand::Money, 50.0f}}));

      // Complex condition with array count
      TEST_TRUE(EvaluateRequirementsFilter(
          FName("QuestsCompleted > 2 OR Money >= 200"),
          {{EReqFilterOperand::QuestsCompleted, TArray<FName>{FName("Quest1"), FName("Quest2"), FName("Quest3")}},
           {EReqFilterOperand::Money, 150.0f}}));

      TEST_TRUE(EvaluateRequirementsFilter(
          FName("QuestsCompleted > 2 OR Money >= 200"),
          {{EReqFilterOperand::QuestsCompleted, TArray<FName>{FName("Quest1")}}, {EReqFilterOperand::Money, 250.0f}}));

      TEST_FALSE(EvaluateRequirementsFilter(
          FName("QuestsCompleted > 2 OR Money >= 200"),
          {{EReqFilterOperand::QuestsCompleted, TArray<FName>{FName("Quest1")}}, {EReqFilterOperand::Money, 150.0f}}));

      // Three-part expression with arrays and money
      TEST_TRUE(EvaluateRequirementsFilter(
          FName("contains(QuestsCompleted, Quest1) AND QuestsCompleted > 2 AND Money >= 100"),
          {{EReqFilterOperand::QuestsCompleted, TArray<FName>{FName("Quest1"), FName("Quest2"), FName("Quest3")}},
           {EReqFilterOperand::Money, 150.0f}}));
    });

    It("should handle special array function syntax correctly", EAsyncExecution::ThreadPool, [this]() {
      // Function-like syntax for contains
      TEST_TRUE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, Quest1)"),
                                           ARRAY_NAMES(FName("Quest1"), FName("Quest2"))));

      TEST_TRUE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, [Quest1, Quest2])"),
                                           ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));

      TEST_FALSE(EvaluateRequirementsFilter(FName("contains(QuestsCompleted, Quest4)"),
                                            ARRAY_NAMES(FName("Quest1"), FName("Quest2"), FName("Quest3"))));
    });
  });
}