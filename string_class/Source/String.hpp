//
//  String.hpp
//  Assignment3
//
//  Created by rick gessner on 1/29/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef String_hpp
#define String_hpp

#include <iostream>
#include "BufferManager.hpp"

//It's up to you to decide the how the string and buffermanager
//will work together -- IS_A vs HAS-A.

namespace ECE141 {

    template<typename T=char, size_t aPresize=32>
    class String: public BufferManager<T> {
    public:

        String(const char* aBuffer=nullptr); //default ctor
        String(const String &aString); //copy constructor

        String& operator=(const String &aCopy);
        String& operator=(const char* aCopy);

        size_t size() const; // returns size of the string stored in buffer

        T& operator[](size_t pos); //mutation method

        String  operator+(const String &aString); // append method using + operator overloading
        String  operator+(const char* aChar); // supports char* inputs
        
        template <typename U, size_t aP>
        friend String operator+(const char* aChar, const String<U, aP> &aString); //supports "hello"+theStringObject

        String& operator+=(const String &aString); // append and assign
        String& operator+=(const char* aChar); // append and assign

        String& insert(size_t anIndex, const String &aStr, size_t aStrIndex, size_t aStrCount); // insert a string/char* into a string
        String& insert(size_t anIndex, T aChar); // insert a char into string

        String& replace(size_t anIndex, size_t aMaxCopyLen, const String &aString); // replace a substring, Supports String and const char* 

        String& erase(size_t anIndex, size_t aCount); // erase a substring, Supports String and const char* 

        //Comparision methods - Supports String and const char* comaprison
        int compare( const String& aString ) const;
        bool operator==(const String &aString) const;
        bool operator!=(const String &aString) const;
        bool operator<(const String &aString) const;
        bool operator<=(const String &aString) const;
        bool operator>(const String &aString) const;
        bool operator>=(const String &aString) const;

        //Search a substring in string
        int find( const String &aString, size_t anIndex = 0 );

        template <typename U, size_t aP>
        friend std::ostream& operator << (std::ostream &anOut, const String<U, aP> &aStr);

        template <typename U, size_t aP>
        friend std::istream& operator>>(std::istream &anOut, String<U, aP> &aString);


    protected:
        size_t length;

        // Helper function to write into a buffer from String.
        void writeFromString(String &target, const String &source, int offset=0);
        // Helper function to write into a buffer from char*.
        void writeFromChar(String &target, const char* source, int offset=0);
        // Helper function to extract a substring from a String
        T* extractSubString(const String &input, size_t aStrIndex, size_t aStrCount);

    };

    // Constructs a String object from a given character buffer.
    template<typename T, size_t aPresize>
    String<T, aPresize>::String(const char* aBuffer): BufferManager<T>(aPresize), length(0){
        if (aBuffer){
            writeFromChar(*this, aBuffer);
        }
    }

    //Copy constructor for a String object.
    template<typename T, size_t aPresize>
    String<T, aPresize>::String(const String &aString): BufferManager<T>(aString), length(aString.length){}

    //Copy assignment operator for String objects.
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::operator=(const String &aCopy){
        BufferManager<T>::operator=(aCopy);
        length = aCopy.length;
        return *this;
    }

    //Copy assignment operator for const char*.
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::operator=(const char* aCopy){
        writeFromChar(*this, aCopy);
        return *this;
    }

    //Get the size of the string stored in the buffer.
    template<typename T, size_t aPresize>
    size_t String<T, aPresize>::size() const{
        return length;
    };

    //Access individual characters in the String by index.
    template<typename T, size_t aPresize>
    T& String<T, aPresize>::operator[](size_t pos){
        T* buffer = this->getBuffer();
        return buffer[pos];
    }

    // Concatenate two String objects using operator overloading.
    template<typename T, size_t aPresize>
    String<T, aPresize> String<T, aPresize>::operator+(const String &aString){
        String out(*this); // Copies the this string object into a new string object
        writeFromString(out, aString, length); // Now appends the RHS string object into above copy object
        return out;
    }

    //Concatenate a String with a char* using operator overloading.
    template<typename T, size_t aPresize>
    String<T, aPresize> String<T, aPresize>::operator+(const char* aChar){
        String out(*this); // Copies the this string object into a new string object
        writeFromChar(out, aChar, length); // Now appends the RHS char* object into above copy object
        return out;
    }

    //Concatenate a char* LHS with a String RHS using operator overloading using friend
    template<typename T, size_t aPresize>
    String<T, aPresize> operator+(const char* aChar, const String<T, aPresize> &aString){
        String out(aChar); // Copies the char* object into a new string object
        writeFromString(out, aString, aString.length); // Now adds char* LHS and String RHS
        return out;
    }

    //Append and assign another String to the current String using operator overloading.
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::operator+=(const String &aString){
        writeFromString(*this, aString, length); // adds and returns the RHS String into this string
        return *this;
    }

    //Append and assign another char* to the current String using operator overloading.
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::operator+=(const char* aChar){
        writeFromChar(*this, aChar, length); // adds and returns the RHS char* into this string
        return *this;
    }


    /**
     * @brief Inserts a substring from another String into the current String at the specified index.
     *
     * This function allows the insertion of a substring from another String starting at a specific position 
     * within the current String. It dynamically adjusts the buffer size to accommodate the new substring.
     * 
     * Supports both String and char* for aStr
     *
     * @param anIndex The index at which the substring should be inserted.
     * @param aStr The source String from which the substring will be extracted.
     * @param aStrIndex The starting index within the source String to begin extracting the substring.
     * @param aStrCount The number of characters to extract from the source String.
     * @return A reference to the modified String object.
     */
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::insert(size_t anIndex, const String &aStr, size_t aStrIndex, size_t aStrCount){
        T* subString = extractSubString(aStr, aStrIndex, aStrCount);
        if(subString){
            size_t req_size = length + aStrCount;
            size_t bCapacity = this->willExpand(req_size);

            if (bCapacity >= req_size) {
                // Shift the existing characters to make space for the inserted substring
                std::memmove(this->getBuffer() + anIndex + aStrCount, this->getBuffer() + anIndex, (length - anIndex + 1) * sizeof(T));

                // Copy the extracted substring into the specified position
                std::memcpy(this->getBuffer() + anIndex, subString, aStrCount * sizeof(T));

                // Update the length of the string
                length += aStrCount;
                this->getBuffer()[length] = '\0';
            }

            // Deallocate memory used for the extracted substring
            delete[] subString;
        }

        return *this;
    }

    /**
     * @brief Inserts a single character into the current String at the specified index.
     *
     * @param anIndex The index at which the character should be inserted.
     * @param aChar The character to be inserted.
     * @return A reference to the modified String object.
     */
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::insert(size_t anIndex, T aChar){        
        if (anIndex > length) {
            return *this; // Invalid index
        }

        // Calculate the required size after the insertion
        size_t req_size = length + 1;
        size_t bCapacity = this->willExpand(req_size);

        if (bCapacity >= req_size) {
            // Shift the existing characters to make space for the inserted character
            std::memmove(this->getBuffer() + anIndex + 1, this->getBuffer() + anIndex, (length - anIndex + 1) * sizeof(T));

            // Insert the new character into the specified position
            this->getBuffer()[anIndex] = aChar;

            // Update the length of the string
            length++;
        }

        return *this;
    }

    /**
     * @brief Replaces a portion of the String with another String starting at a specified index.
     *
     * Supports both String and char* for aString
     * @param anIndex The index at which the replacement should begin.
     * @param aMaxCopyLen The maximum number of characters to replace.
     * @param aString The String providing the replacement content.
     * @return A reference to the modified String object.
     */
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::replace(size_t anIndex, size_t aMaxCopyLen, const String &aString){
        // Ensure the index is within bounds
        if (anIndex > length) {
            return *this; // Invalid index or zero copy length
        }

        // Calculate the actual number of characters to replace
        // size_t replaceCount = std::min(aMaxCopyLen, length - anIndex);

        // Erase the specified portion to be replaced
        erase(anIndex, aMaxCopyLen);
        // Insert the contents of the replacement string at the specified index
        insert(anIndex, aString, 0, aString.size());
        return *this;
    }

    /**
     * @brief Erases a portion of the String starting from a specified index.
     *
     * @param anIndex The index at which the erasure should begin.
     * @param aCount The number of characters to erase.
     * @return A reference to the modified String object.
     */
    template<typename T, size_t aPresize>
    String<T, aPresize>& String<T, aPresize>::erase(size_t anIndex, size_t aCount){

        if (anIndex >= length || aCount == 0) {
            return *this; // Invalid index or count
        }

        // Calculate the number of characters to erase
        size_t eraseCount = std::min(aCount, length - anIndex);

        // Shift the remaining characters to fill the erased section
        std::memmove(this->getBuffer() + anIndex, this->getBuffer() + anIndex + eraseCount, (length - anIndex - eraseCount + 1) * sizeof(T));

        // Update the length of the string
        length -= eraseCount;
        return *this;
    }

    /**
     * @brief Compares the current String with another String.
     *
     * It uses std::strcmp to perform the comparison.
     *
     * @param aString The String to compare with.
     * @return An integer value indicating the result of the comparison:
     *         - 0 if the strings are equal.
     *         - A negative value if the current String is lexicographically less than the other String.
     *         - A positive value if the current String is lexicographically greater than the other String.
     */
    template<typename T, size_t aPresize>
    int String<T, aPresize>::compare(const String& aString ) const{
        return std::strcmp(this->getBuffer(), aString.getBuffer());
    }


    /**
     * @brief Finds the first occurrence of a substring within the current String.
     *
     *
     * @param aString The substring to search for.
     * @param anIndex The index from which the search should begin.
     * @return The index of the first character of the found substring, or -1 if the substring is not found.
     */
    template<typename T, size_t aPresize>
    int String<T, aPresize>::find(const String &aString, size_t anIndex){
        if (anIndex >= length) {
            return -1; // Invalid starting index
        }

        // Find the first occurrence of the substring starting from the specified index
        const char* substringPtr = std::strstr(this->getBuffer() + anIndex, aString.getBuffer());

        // If the substring is found, calculate and return the index
        if (substringPtr != nullptr) {
            size_t foundIndex = substringPtr - this->getBuffer();
            return static_cast<int>(foundIndex);
        }

        // If the substring is not found, return -1
        return -1;
    }

    // Comparsion operators using the compare function
    template<typename T, size_t aPresize>
    bool String<T, aPresize>::operator==(const String& aString) const {
        return compare(aString) == 0;
    }

    template<typename T, size_t aPresize>
    bool String<T, aPresize>::operator!=(const String& aString) const {
        return compare(aString) != 0;
    }

    template<typename T, size_t aPresize>
    bool String<T, aPresize>::operator<(const String& aString) const {
        return compare(aString) < 0;
    }

    template<typename T, size_t aPresize>
    bool String<T, aPresize>::operator<=(const String& aString) const {
        return compare(aString) <= 0;
    }

    template<typename T, size_t aPresize>
    bool String<T, aPresize>::operator>(const String& aString) const {
        return compare(aString) > 0;
    }

    template<typename T, size_t aPresize>
    bool String<T, aPresize>::operator>=(const String& aString) const {
        return compare(aString) >= 0;
    }


    template<typename U, size_t aP>
    std::ostream& operator<<(std::ostream &anOut, const String<U, aP> &aStr) {
        for (size_t i = 0; i < aStr.size(); ++i) {
            anOut << aStr[i];
        }
        anOut << "\n";
        return anOut;
    }
    
    template<typename U, size_t aP>
    std::istream& operator >> (std::istream &anOut,  String<U, aP> &aString){
        // Clear the existing content of the string
        aString = String<U, aP>();

        // Temporary buffer to read characters
        char* buffer[aP];
        
        // Read characters until a newline is encountered or the buffer is full
        anOut.getline(buffer, aP);

        // Write the characters from the buffer to the string
        aString = buffer;

        return anOut;
    }

    /**
     * @brief Writes the contents of one String into another String buffer at a specified offset.
     *
     * @param target The String object to which the content will be written.
     * @param source The source String providing the content to be written.
     * @param offset The offset in the target String where the content should be written.
     */
    template<typename T, size_t aPresize>
    void String<T, aPresize>::writeFromString(String &target, const String &source, int offset){
        // The new total length of the target
        target.length = target.length + source.length;

        // Expanding the buffer if required
        size_t bCapacity = target.willExpand(target.length);

        // Copying the source string into target buffer after a specified offset
        if(bCapacity >= target.length){
            std::memcpy(target.getBuffer()+offset, source.getBuffer(), source.length);
            target.getBuffer()[target.size()] = '\0';
        }
        else{
            throw "Problem with Buffer";
        }
    }

    /**
     * @brief Writes the contents of a char* into another String buffer at a specified offset.
     *
     * @param target The String object to which the content will be written.
     * @param source The source char* providing the content to be written.
     * @param offset The offset in the target String where the content should be written.
     */
    template<typename T, size_t aPresize>
    void String<T, aPresize>::writeFromChar(String &target, const char* source, int offset){
        // The new total length of the target
        target.length = target.length + std::strlen(source);

        // Expanding the buffer if required
        size_t bCapacity = target.willExpand(target.length);

        // Copying the source char* into target buffer after a specified offset
        if(bCapacity >= target.length){
            std::strcpy(target.getBuffer()+offset, source);
            target.getBuffer()[target.size()] = '\0';
        }
        else{
            throw "Problem with Buffer";
        }
    }

    /**
     * @brief Extracts a substring from a given String based on specified indices and count.
     *
     * @param input The input String from which the substring will be extracted.
     * @param aStrIndex The starting index within the input String to begin extraction.
     * @param aStrCount The number of characters to extract from the input String.
     * @return A pointer to the allocated memory containing the extracted substring.
     *         Returns nullptr if the input is invalid or extraction fails.
     */
    template<typename T, size_t aPresize>
    T* String<T, aPresize>::extractSubString(const String &input, size_t aStrIndex, size_t aStrCount){
        if (input.getBuffer() == nullptr || aStrIndex >= input.size() || aStrCount == 0) {
            return nullptr; // Invalid input
        }
        
        // Calculate the actual length of the substring to extract
        size_t substringLength = std::min(input.size() - aStrIndex, aStrCount);

        // Allocate memory for the extracted substring
        T* substring = new T[substringLength + 1];

        // Copy the substring from the input String to the allocated memory
        std::strncpy(substring, input.getBuffer() + aStrIndex, substringLength);

        // Add null-terminator to the end of the substring
        // substring[substringLength] = '\0';
        return substring;
    }

} // namespace ECE141

#endif /* String_hpp */