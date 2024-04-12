//
// Created by rrr on 2/20/24.
//
#include "Model.h"

namespace ECE141{

    /**
     * @brief Detects the type of value in the given string and sets the nodeType and nodeValue accordingly.
     *
     * This function analyzes the input string and determines its type, such as integer, floating-point number,
     * boolean, or string. It then sets the nodeType and nodeValue members of the ModelNode instance based on
     * the detected type. If the string does not represent a recognized type, it is treated as a string.
     *
     * @param value The input string to be analyzed.
     * @return void
     */
    void ModelNode::detectValueType(std::string value) {
        orgValue = value;
        std::optional<int>intValue = intChecker(value);
        if (intValue.has_value()){
            nodeValue = intValue;
            nodeType = NodeType::Int;
            return;
        }

        std::optional<long int>longValue = longChecker(value);
        if (longValue.has_value()){
            nodeValue = longValue;
            nodeType = NodeType::LongInt;
            return;
        }

        std::optional<float>floatValue = floatChecker(value);
        if (floatValue.has_value()){
            nodeValue = floatValue;
            nodeType = NodeType::Float;
            return;
        }

        std::optional<double>doubleValue = doubleChecker(value);
        if (doubleValue.has_value()){
            nodeValue = doubleValue;
            nodeType = NodeType::Double;
            return;
        }

        std::optional<bool>boolValue = boolChecker(value);
        if (boolValue.has_value()){
            nodeValue = boolValue;
            nodeType = NodeType::Bool;
            return;
        }

        nodeValue = value;
        nodeType = NodeType::String;

    }

    /**
     * @brief Checks if a given string can be converted to an integer.
     *
     * This function attempts to convert the input string to an integer using std::stoi.
     * It returns std::nullopt if the conversion fails due to an invalid argument or
     * if the value is out of the representable range for an int.
     *
     * @param value The input string to be checked.
     * @return std::optional<int> The converted integer value or std::nullopt on failure.
     */
    std::optional<int> ModelNode::intChecker(const std::string& value) {
        size_t stringLength{value.length()};
        size_t pos;
        try {
            int intValue = std::stoi(value, &pos);
            if (pos == stringLength) {
                return intValue;
            }
        } catch (const std::invalid_argument&) {
            return std::nullopt;
        } catch (const std::out_of_range&) {
            return std::nullopt;
        }
        return std::nullopt;
    }

    /**
     * @brief Checks if a given string can be converted to an long integer.
     *
     * This function attempts to convert the input string to an long integer using std::stol.
     * It returns std::nullopt if the conversion fails due to an invalid argument or
     * if the value is out of the representable range for an long int.
     *
     * @param value The input string to be checked.
     * @return std::optional<int> The converted long integer value or std::nullopt on failure.
     */
    std::optional<long> ModelNode::longChecker(const std::string& value) {
        size_t stringLength{value.length()};
        size_t pos;
        try {
            long int longValue = std::stol(value, &pos);
            if (pos == stringLength) {
                return longValue;
            }
        } catch (const std::invalid_argument&) {
            return std::nullopt;
        } catch (const std::out_of_range&) {
            return std::nullopt;
        }
        return std::nullopt;
    }

    /**
     * @brief Checks if a given string can be converted to an float.
     *
     * This function attempts to convert the input string to an float using std::stof.
     * It returns std::nullopt if the conversion fails due to an invalid argument or
     * if the value is out of the representable range for an float.
     *
     * @param value The input string to be checked.
     * @return std::optional<int> The converted float value or std::nullopt on failure.
     */
    std::optional<float> ModelNode::floatChecker(const std::string& value) {
        size_t stringLength{value.length()};
        size_t pos;
        try {
            float floatValue = std::stof(value, &pos);
            if (pos == stringLength) {
                return floatValue;
            }
        } catch (const std::invalid_argument&) {
            return std::nullopt;
        } catch (const std::out_of_range&) {
            return std::nullopt;
        }
        return std::nullopt;
    }

    /**
     * @brief Checks if a given string can be converted to an double.
     *
     * This function attempts to convert the input string to an double using std::stof.
     * It returns std::nullopt if the conversion fails due to an invalid argument or
     * if the value is out of the representable range for an double.
     *
     * @param value The input string to be checked.
     * @return std::optional<int> The converted double value or std::nullopt on failure.
     */
    std::optional<double> ModelNode::doubleChecker(const std::string& value) {
        size_t stringLength{value.length()};
        size_t pos;
        try {
            double doubleValue = std::stod(value, &pos);
            if (pos == stringLength) {
                return doubleValue;
            }
        } catch (const std::invalid_argument&) {
            return std::nullopt;
        } catch (const std::out_of_range&) {
            return std::nullopt;
        }
        return std::nullopt;
    }

    /**
     * @brief Checks if a given string can be converted to a boolean.
     *
     * This function checks if the input string is equal to "true" or "false" and
     * returns the corresponding boolean value. If the input is not a valid boolean
     * representation, it returns std::nullopt.
     *
     * @param value The input string to be checked.
     * @return std::optional<bool> The converted boolean value or std::nullopt on failure.
     */
    std::optional<bool> ModelNode::boolChecker(const std::string& value) {
        if (value == "true" || value == "false") {
            bool boolValue = (value == "true");
            return boolValue;
        }
        else {
            return std::nullopt;
        }
    }
}