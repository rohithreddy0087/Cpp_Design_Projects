#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <sstream>

namespace ECE141
{
    class Testable
    {
    protected:
        // Make sure this variable gets incremented each time a test is added.
        size_t theNumberOfTests = 0;

    public:
        Testable() = default;
        virtual ~Testable() = default;

        // Retrieve the name of a specific test.
        virtual std::optional<std::string> getTestName(size_t anIndex) const = 0;

        // Run a specific test.
        virtual bool operator()(const std::string &aName) = 0;

        // Run all tests, returns number of tests passed.
        size_t runAllTests() {
            size_t theNumberOfTestsPassed = 0;
            std::stringstream theOutput;
            for(size_t i = 0; i < theNumberOfTests; i++) {
                if (auto theName = getTestName(i)) {
                    const bool hasPassed = (*this)(theName.value());
                    theNumberOfTestsPassed += (int)hasPassed;

                    theOutput << i + 1 << ". " << theName.value() << ": "
                              << (hasPassed ? "PASS" : "FAIL") << "\n";
                }
            }

            if (theNumberOfTestsPassed == theNumberOfTests)
                std::cout << "All";
            else
                std::cout << theNumberOfTestsPassed << " of " << theNumberOfTests;

            std::cout << " tests passed.\n" << theOutput.str() << "\n";
            return theNumberOfTestsPassed;
        }

    };

} // namespace ECE141
