#include "Testable.h"
#include "Variant.h"
#include <vector>
#include <functional>

namespace ECE141 {

    class VariantTester : public Testable {
    public:
        VariantTester() {
            tests.push_back({"Constructor Tests", std::bind(&VariantTester::ConstructorTests, this)});
            tests.push_back({"Assignment Operator Tests", std::bind(&VariantTester::AssignmentOperatorTests, this)});
            tests.push_back({"Variant Methods Tests", std::bind(&VariantTester::VariantMethodsTests, this)});
            tests.push_back({"Comparison Tests", std::bind(&VariantTester::ComparisonTests, this)});
            theNumberOfTests = tests.size();
        }

        virtual ~VariantTester() = default;

        std::optional<std::string> getTestName(size_t anIndex) const override {
            if (anIndex < tests.size()) {
                return tests[anIndex].first;
            }
            return std::nullopt;
        }

        bool operator()(const std::string &aName) override {
            for (auto &test : tests) {
                if (test.first == aName) {
                    return test.second();
                }
            }
            return false;
        }

    private:
        std::vector<std::pair<std::string, std::function<bool()>>> tests;

        bool ConstructorTests() {
            ECE141::Variant theV1(100);
            ECE141::Variant theV2(6.28f);
            ECE141::Variant theV3("testing");
            
            std::stringstream theOut;
            theOut << theV1 << " " << std::fixed
                    << theV2 << " " << theV3 << "\n";
            std::stringstream theIn1(theOut.str());
            int theInt;
            std::string theString1, theString2;
            theIn1 >> theInt >> theString1 >> theString2;
            theString1.erase(theString1.find_last_not_of('0') + 1, std::string::npos);
            if (theInt != 100 || theString1 != "6.28" || theString2 != "testing")
                return false;

            ECE141::Variant theV4("123");
            ECE141::Variant theV5("6.145898f");
            ECE141::Variant theV6("test123");
            
            std::stringstream theOut2;
            theOut2 << theV4 << " " << std::fixed
                    << theV5 << " " << theV6 << "\n";
            std::stringstream theIn2(theOut2.str());
            std::string theString3, theString4, theString5;
            theIn2 >> theString5 >> theString3 >> theString4;
            theString5.erase(theString5.find_last_not_of('0') + 1, std::string::npos);
            theString3.erase(theString3.find_last_not_of('0') + 1, std::string::npos);
            if (theString5 != "123" || theString3 != "6.145898f" || theString4 != "test123")
                return false;

            ECE141::Variant theV7(2147483647);
            return true;
        }

        bool AssignmentOperatorTests(){

            ECE141::Variant theV1(100);
            ECE141::Variant theV2(6.28f);
            ECE141::Variant theV3("testing");

            std::stringstream theOut;
            theOut.str(std::string());
            ECE141::Variant theVar(theV1); // Invoke the copy-constructor
            theVar = 200;
            theOut << theVar << " ";
            theVar = 3.14f;
            theOut << std::fixed << theVar << " ";
            theVar = "again";
            theOut << std::fixed << theVar << "\n";
            std::stringstream theIn2(theOut.str());
            int theInt;
            std::string theString1, theString2;
            theIn2 >> theInt >> theString1 >> theString2;
            theString1.erase(theString1.find_last_not_of('0') + 1, std::string::npos);
            if (theInt != 200 || theString1 != "3.14" || theString2 != "again")
                return false;

            return true;
        }

        bool VariantMethodsTests(){

            Variant v1(-2147);
            Variant v2(2147483647); // Max int
            Variant v3(-0);

            if (!(v1.asFloat() && v1.asFloat().value() == -2147.0f)) return false;
            if (!(v2.asFloat() && v2.asFloat().value() == 2147483647.0f)) return false;
            if (!(v3.asFloat() && v3.asFloat().value() == -0)) return false;

            if (!(v1.asString() && v1.asString().value() == "-2147")) return false;
            if (!(v2.asString() && v2.asString().value() == "2147483647")) return false;
            if (!(v3.asString() && v3.asString().value() == "0")) return false;

            // Variant v1("Hello");
            // Variant v1("123");
            Variant v4(-2147.785f);
            Variant v5(2147483.12345f);
            Variant v6(-0.0f);
            if (!(v4.asInt() && v4.asInt().value() == -2147)) return false;
            if ((v5.asInt() && v5.asInt().value() == 2147483647)) return false;
            if (!(v6.asInt() && v6.asInt().value() == 0)) return false;
            if (!(v4.asString() && v4.asString().value() == std::to_string(-2147.785f))) return false;
            if (!(v5.asString() && v5.asString().value() == std::to_string(2147483.12345f))) return false;
            if (!(v6.asString() && v6.asString().value() == std::to_string(-0.0f))) return false;

            Variant v7("Hello");
            Variant v8("123");
            Variant v9("123.569");
            Variant v10("abc123");
            if ((v7.asInt().has_value())) return false;
            if (!(v8.asInt() && v8.asInt().value() == 123)) return false;
            if (!(v9.asInt() && v9.asInt().value() == 123)) return false;
            if ((v10.asInt().has_value())) return false;

            if ((v7.asFloat().has_value())) return false;
            if (!(v8.asFloat() && v8.asFloat().value() == 123.0f)) return false;
            if (!(v9.asFloat() && v9.asFloat().value() == 123.569f)) return false;
            if ((v10.asFloat().has_value())) return false;

            return true;
        }

        bool ComparisonTests(){

            Variant v1(-2147);
            Variant v2(2147483647); // Max int
            Variant v3(-0);

            Variant v4(-2147.785f);
            Variant v5(2147483.12345f);
            Variant v6(-0.0f);

            Variant v7("Hello");
            Variant v8("123");
            Variant v9("123.569");
            Variant v10("abc123");

            if (!(v1==v4)) return false;
            if (!(v4<=v1)) return false;
            if (!(v2>v4)) return false;
            if (!(v3>=v6)) return false;
            if (!(v1<=v5)) return false;
            if ((v1==v7)) return false;
            if (!(v8<v2)) return false;
            if (!(v7!=v10)) return false;

            return true;
        }
    };

}
