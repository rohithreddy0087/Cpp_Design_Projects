#include "Variant.h"

namespace ECE141
{   
    Variant::Variant() {}
    
    Variant::Variant(int aValue) {
        initFromInt(aValue);      
    }

    Variant::Variant(float aValue) {
        initFromFloat(aValue);
    }

    Variant::Variant(const char* aValue) {
        initFromString(aValue);
    }

    Variant::Variant(const Variant & aCopy){
        copyVariant(aCopy);
    }

    Variant& Variant::operator=(const Variant & aCopy){
        copyVariant(aCopy);
        return *this;
    }

    Variant& Variant::operator=(int aValue){
        initFromInt(aValue);  
        return *this;
    }

    Variant& Variant::operator=(float aValue){
        initFromFloat(aValue);
        return *this;
    }
    
    Variant& Variant::operator=(const char* aValue){
        initFromString(aValue);
        return *this;
    }

    std::optional<int> Variant::asInt() const{
        if (intValue.has_value()) {return intValue.value();}
        return std::nullopt;
    }

    std::optional<float> Variant::asFloat() const{
        if (floatValue.has_value()) {return floatValue.value();}
        return std::nullopt;
    }

    std::optional<std::string> Variant::asString() const{
        if (charValue.has_value()) {return charValue.value();}
        return std::nullopt;
    }

    bool Variant::operator==(const Variant& other) {
        return compareVariants(other, std::equal_to<>{});
    }

    bool Variant::operator!=(const Variant& other) {
        return compareVariants(other, std::not_equal_to<>{});
    }

    bool Variant::operator<(const Variant& other) {
        return compareVariants(other, std::less<>{});
    }

    bool Variant::operator<=(const Variant& other) {
        return compareVariants(other, std::less_equal<>{});
    }

    bool Variant::operator>(const Variant& other) {
        return compareVariants(other, std::greater<>{});
    }

    bool Variant::operator>=(const Variant& other) {
        return compareVariants(other, std::greater_equal<>{});
    }

    VariantType Variant::getType() const{
        return type;
    }

    void Variant::initFromInt(int aValue) {
        intValue = aValue;
        type = VariantType::intType;
        floatValue = static_cast<float>(aValue);
        charValue = std::to_string(aValue);
    }

    void Variant::initFromFloat(float aValue) {
        floatValue = aValue;
        type = VariantType::floatType;
        intValue = static_cast<int>(aValue);
        charValue = std::to_string(aValue);
    }

    void Variant::initFromString(const char* aValue) {
        charValue = std::string(aValue);
        type = VariantType::stringType;
        if(!containsAlphabets(charValue.value())){
            intValue = std::stoi(charValue.value());
            floatValue = std::stof(charValue.value());
        }
        else{
            intValue = std::nullopt;
            floatValue = std::nullopt;
        }
    }

    template<typename Comparator>
    bool Variant::compareVariants(const Variant& other, Comparator comp) const {
        switch (type) {
            case VariantType::intType:
                if (other.asInt().has_value()){return comp(intValue.value(), other.asInt().value());}
                else {return false;}

            case VariantType::floatType:
                if (other.asFloat().has_value()){return comp(floatValue.value(), other.asFloat().value());}
                else {return false;}

            case VariantType::stringType:
                if (other.asString().has_value()){return comp(charValue.value(), other.asString().value());}
                else {return false;}
            default:
                throw std::runtime_error("Unhandled Variant Type");
        }
    }

    void Variant::copyVariant(const Variant & aCopy){
        if (aCopy.asInt().has_value()){intValue = aCopy.asInt().value();}
        else{intValue = std::nullopt;}

        if (aCopy.asFloat().has_value()){floatValue = aCopy.asFloat().value();}
        else{floatValue = std::nullopt;}

        if (aCopy.asString().has_value()){charValue = aCopy.asString().value();}
        else{charValue = std::nullopt;}

        type = aCopy.getType();
    }

    bool Variant::containsAlphabets(std::string inp) const{
        for (char ch : inp) {
            if (std::isalpha(ch)) {
                return true;
            }
        }
        return false;
    }

    Variant::~Variant(){}

    std::ostream& operator<<(std::ostream& aStream, const Variant& aVariant) {
        switch (aVariant.getType()) {
            case VariantType::intType:{
                aStream << aVariant.asInt().value();
                break;
            }
            case VariantType::floatType:{
                aStream << aVariant.asFloat().value();
                break;
            }
            case VariantType::stringType:{
                aStream << aVariant.asString().value();
                break;
            }
        }
        return aStream;
    }
} // namespace ECE141