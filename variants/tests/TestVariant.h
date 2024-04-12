#include "Testable.h"
#include "Variant.h"
#include <vector>
#include <functional>

namespace ECE141 {

    class VariantTester : public Testable {
    public:
        VariantTester() {
            tests.push_back({"TestIntConstruction", std::bind(&VariantTester::testIntConstruction, this)});
            tests.push_back({"TestFloatConstruction", std::bind(&VariantTester::testFloatConstruction, this)});
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

        bool testIntConstruction() {
            Variant v(42);
            return v.asInt() && v.asInt().value() == 42;
        }

        bool testFloatConstruction() {
            Variant v(3.14f);
            return v.asFloat() && v.asFloat().value() == 3.14f;
        }

    };

}
