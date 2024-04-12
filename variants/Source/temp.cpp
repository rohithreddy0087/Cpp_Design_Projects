// #include "Variant.h"

// namespace ECE141
// {
//     Variant::Variant(int aValue) {
//         this->intValue = aValue;
//         this->type = VariantType::intType;
//         if(this->asFloat().has_value()){
//             this->floatValue = this->asFloat().value();
//         }
//         if(this->asString().has_value()){
//             this->charValue = this->asString().value();
//         }
        
//     }

//     Variant::Variant(float aValue) {
//         this->floatValue = aValue;
//         this->type = VariantType::floatType;
//         if(this->asInt().has_value()){
//             this->intValue = this->asInt().value();
//         }
//         if(this->asString().has_value()){
//             this->charValue = this->asString().value();
//         }
//     }

//     Variant::Variant(const char* aValue) {
//         this->charValue = std::string(aValue);
//         this->type = VariantType::stringType;
//         if(this->asInt().has_value()){
//             this->intValue = this->asInt().value();
//         }
//         if(this->asFloat().has_value()){
//             this->floatValue = this->asFloat().value();
//         }
//     }

//     Variant::Variant(const Variant & aCopy){
//         this->operator=(aCopy);
//     }

//     // Variant& Variant::operator=(const Variant & aCopy){
//     //     this->operator=(aCopy);
//     //     return *this;
//     // }

//     Variant& Variant::operator=(int aValue){
//         this->intValue = aValue;
//         this->type = VariantType::intType;
//         if(this->asFloat().has_value()){
//             this->floatValue = this->asFloat().value();
//         }
//         if(this->asString().has_value()){
//             this->charValue = this->asString().value();
//         }
//         return *this;
//     }

//     Variant& Variant::operator=(float aValue){
//         this->floatValue = aValue;
//         this->type = VariantType::floatType;
//         if(this->asInt().has_value()){
//             this->intValue = this->asInt().value();
//         }
//         if(this->asString().has_value()){
//             this->charValue = this->asString().value();
//         }
//         return *this;
//     }
    
//     Variant& Variant::operator=(const char* aValue){
//         this->charValue = std::string(aValue);
//         this->type = VariantType::stringType;
//         if(this->asInt().has_value()){
//             this->intValue = this->asInt().value();
//         }
//         if(this->asFloat().has_value()){
//             this->floatValue = this->asFloat().value();
//         }
//         return *this;
//     }

//     std::optional<int> Variant::asInt() const{
//        switch (this->type) {
//             case VariantType::intType:
//                 return this->intValue;
//             case VariantType::floatType:
//                 return static_cast<int>(this->floatValue);
//             case VariantType::stringType:{
//                 if(!containsAlphabets(this->charValue)){
//                     return std::stoi(this->charValue);
//                 }
//                 else{
//                     return std::nullopt;
//                 }
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//        } 
//     }

//     std::optional<float> Variant::asFloat() const{
//        switch (this->type) {
//             case VariantType::intType:
//                 return static_cast<float>(this->intValue);
//             case VariantType::floatType:
//                 return this->floatValue;
//             case VariantType::stringType:{
//                 if(!containsAlphabets(this->charValue)){
//                     return std::stof(this->charValue);
//                 }
//                 else{
//                     return std::nullopt;
//                 }
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//        } 
//     }

//     std::optional<std::string> Variant::asString() const{
//        switch (this->type) {
//             case VariantType::intType:
//                 return std::to_string(this->intValue);
//             case VariantType::floatType:
//                 return std::to_string(this->floatValue);
//             case VariantType::stringType:{
//                 return this->charValue;
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//        } 
//     }

//     bool Variant::operator==(const Variant& other){
//         switch (this->type) {
//             case VariantType::intType:{
//                 if(other.asInt().has_value()) return this->intValue == other.asInt().value();
//                 else return false;
//             }
//             case VariantType::floatType:{
//                 if(other.asFloat().has_value()) return this->floatValue == other.asFloat().value();
//                 else return false;
//             }
//             case VariantType::stringType:{
//                 if(other.asString().has_value()) return this->charValue == other.asString().value();
//                 else return false;
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//         }
//     }

//     bool Variant::operator!=(const Variant& other){
//         switch (this->type) {
//             case VariantType::intType:{
//                 if(other.asInt().has_value()) return this->intValue != other.asInt().value();
//                 else return false;
//             }
//             case VariantType::floatType:{
//                 if(other.asFloat().has_value()) return this->floatValue != other.asFloat().value();
//                 else return false;
//             }
//             case VariantType::stringType:{
//                 if(other.asString().has_value()) return this->charValue != other.asString().value();
//                 else return false;
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//         }
//     }

//     bool Variant::operator<(const Variant& other){
//         switch (this->type) {
//             case VariantType::intType:{
//                 if(other.asInt().has_value()) return this->intValue < other.asInt().value();
//                 else return false;
//             }
//             case VariantType::floatType:{
//                 if(other.asFloat().has_value()) return this->floatValue < other.asFloat().value();
//                 else return false;
//             }
//             case VariantType::stringType:{
//                 if(other.asString().has_value()) return this->charValue < other.asString().value();
//                 else return false;
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//         }
//     }

//     bool Variant::operator<=(const Variant& other){
//         switch (this->type) {
//             case VariantType::intType:{
//                 if(other.asInt().has_value()) return this->intValue <= other.asInt().value();
//                 else return false;
//             }
//             case VariantType::floatType:{
//                 if(other.asFloat().has_value()) return this->floatValue <= other.asFloat().value();
//                 else return false;
//             }
//             case VariantType::stringType:{
//                 if(other.asString().has_value()) return this->charValue <= other.asString().value();
//                 else return false;
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//         }
//     }

//     bool Variant::operator>(const Variant& other){
//         switch (this->type) {
//             case VariantType::intType:{
//                 if(other.asInt().has_value()) return this->intValue > other.asInt().value();
//                 else return false;
//             }
//             case VariantType::floatType:{
//                 if(other.asFloat().has_value()) return this->floatValue > other.asFloat().value();
//                 else return false;
//             }
//             case VariantType::stringType:{
//                 if(other.asString().has_value()) return this->charValue > other.asString().value();
//                 else return false;
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//         }
//     }

//     bool Variant::operator>=(const Variant& other){
//         switch (this->type) {
//             case VariantType::intType:{
//                 if(other.asInt().has_value()) return this->intValue >= other.asInt().value();
//                 else return false;
//             }
//             case VariantType::floatType:{
//                 if(other.asFloat().has_value()) return this->floatValue >= other.asFloat().value();
//                 else return false;
//             }
//             case VariantType::stringType:{
//                 if(other.asString().has_value()) return this->charValue >= other.asString().value();
//                 else return false;
//             }
//             default:
//                 throw std::runtime_error("Unhandled Variant Type");
//         }
//     }

//     VariantType Variant::getType() const{
//         return this->type;
//     }

//     bool Variant::containsAlphabets(std::string inp) const{
//         const char* str = inp.c_str();
//         if (str == nullptr) return false;
//         while (*str) {
//             if ((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z')) {
//                 return true;
//             }
//             ++str;
//         }
//         return false;
//     }

//     Variant::~Variant(){}

//     std::ostream& operator<<(std::ostream& aStream, const Variant& aVariant) {
//         // aStream << this->intValue <<"\n";
//         // aStream << this->floatValue <<"\n";
//         // aStream << this->charValue <<"\n";
//         switch (aVariant.getType()) {
//             case VariantType::intType:{
//                 aStream << aVariant.asInt().value();
//                 break;
//             }
//             case VariantType::floatType:{
//                 aStream << aVariant.asFloat().value();
//                 break;
//             }
//             case VariantType::stringType:{
//                 aStream << aVariant.asString().value();
//                 break;
//             }
//         }
//         return aStream;
//     }
// } // namespace ECE141