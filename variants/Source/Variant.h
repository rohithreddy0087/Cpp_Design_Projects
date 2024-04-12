#pragma once
#include <iostream>
#include <optional>
#include <string>

namespace ECE141
{   
    enum class VariantType { intType, floatType, stringType };

    class Variant
    {
    public:
        // ---Constructors---
        Variant();
        Variant(int aValue);
        Variant(float aValue);
        Variant(const char* aValue);
        Variant(const Variant & aCopy);
        ~Variant();

        // ---TODO: Add other OCF methods---
        // Variant& operator=(const Variant & aCopy);
        Variant& operator=(int aValue);
        Variant& operator=(float aValue);
        Variant& operator=(const char* aValue);
        Variant& operator=(const Variant & aCopy);

        // Comparison Operators
        bool operator==(const Variant& other);
        bool operator!=(const Variant& other);
        bool operator<(const Variant& other);
        bool operator<=(const Variant& other);
        bool operator>(const Variant& other);
        bool operator>=(const Variant& other);

        // ---TODO: Add variant methods---
        std::optional<int> asInt() const;            // Retrieve value as int
        std::optional<float> asFloat() const;        // Retrieve value as float
        std::optional<std::string> asString() const; // Retrieve value as string
        VariantType getType() const; // Get current type of variant

        friend std::ostream& operator<<(std::ostream& aStream, const Variant& aVariant);

    protected:
        // ---TODO: Add member variables---
        
        std::optional<int> intValue;
        std::optional<float> floatValue;
        std::optional<std::string> charValue;
        VariantType type;

        void initFromInt(int aValue);
        void initFromFloat(float aValue);
        void initFromString(const char* aValue);
        void copyVariant(const Variant & aCopy);
        bool containsAlphabets(std::string str) const;
        template<typename Comparator>
        bool compareVariants(const Variant& other, Comparator comp) const;
        // Variant& convertType(Variant& other);
    };

} // namespace ECE141