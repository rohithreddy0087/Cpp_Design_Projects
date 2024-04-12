//
//  BufferManager.hpp
//  Assignment2
//
//

#ifndef BufferManager_h
#define BufferManager_h
#include <iostream>
#include <cstring>

namespace ECE141   {

    template <typename T>
    class BufferManager {
    public:
        BufferManager();
        BufferManager(const BufferManager &aCopy);
        BufferManager(size_t aSize);
        ~BufferManager();

        BufferManager& operator=(const BufferManager &aCopy); // Copy assignment operator: Assigns the contents of another BufferManager.


        size_t getCapacity() const;
        T*     getBuffer() const;
        size_t willExpand(size_t aNewSize, size_t anOffset=0);
        size_t willCompact(size_t aNewSize, size_t anOffset=0);

    private:
        size_t bufferSize;
        size_t bufferCapacity;
        std::unique_ptr<T[]> bufferPtr; // Smart pointer to the dynamically allocated buffer.
        
        void changeCapacity(size_t newCapacity);  // Helper function to change the capacity of the buffer

    };

    template <typename T>
    BufferManager<T>::BufferManager(): bufferSize(0), bufferCapacity(0), bufferPtr(nullptr){}

    template <typename T>
    BufferManager<T>::BufferManager(const BufferManager &aCopy): bufferSize(aCopy.bufferSize), bufferCapacity(aCopy.bufferCapacity){
        bufferPtr = std::make_unique<T[]>(bufferCapacity);
        std::memcpy(bufferPtr.get(), aCopy.bufferPtr.get(), bufferSize); //copies only buffersize instead of whole buffer
    }

    template <typename T>
    BufferManager<T>::BufferManager(size_t aSize): bufferSize(0), bufferCapacity(aSize*sizeof(T)){
        bufferPtr = std::make_unique<T[]>(bufferCapacity);
    }

    template <typename T>
    BufferManager<T>::~BufferManager(){}

    template <typename T>
    BufferManager<T>& BufferManager<T>::operator=(const BufferManager &aCopy){
        bufferSize = aCopy.bufferSize;
        bufferCapacity = aCopy.bufferCapacity;
        bufferPtr = std::make_unique<T[]>(bufferSize);
        std::memcpy(bufferPtr.get(), aCopy.bufferPtr.get(), bufferSize);
        return *this;
    }

    // Get the current capacity of the buffer.
    template <typename T>
    size_t BufferManager<T>::getCapacity() const {
        return bufferCapacity;
    }

    // Get a pointer to the underlying buffer.
    template <typename T>
    T* BufferManager<T>::getBuffer() const {
        return bufferPtr.get();
    }

    /**
     * @brief Dynamically adjusts the buffer capacity to accommodate a specified new size. If the buffer is initially empty, 
     * it directly sets the capacity to the new size. Otherwise, it doubles the current capacity iteratively until it exceeds
     * the new size.
     *
     * @param aNewSize The desired new size of the buffer.
     * @param anOffset The offset for the expansion if needed (default is 0).
     * @return The updated capacity of the buffer after expansion.
    */
    template <typename T>
    size_t BufferManager<T>::willExpand(size_t aNewSize, [[maybe_unused]] size_t anOffset){
        bufferSize = aNewSize;
        if (bufferCapacity == 0){
            changeCapacity(aNewSize);
            return bufferCapacity;
        }
        if (aNewSize > bufferCapacity) {
            size_t newCapacity = bufferCapacity;
            while (newCapacity < aNewSize) {
                newCapacity *= 2;
            }
            if (newCapacity > bufferCapacity) {
                changeCapacity(newCapacity);
            }
        }
        return bufferCapacity;
    }

    /**
     * @brief Similar to willExpand function, but halves the current capacity iteratively until it is less than
     * the new size.
     *
     * @tparam T The type of elements stored in the buffer.
     * @param aNewSize The desired new size of the buffer.
     * @param anOffset The offset for the compaction if needed (default is 0).
     * @return The updated capacity of the buffer after compaction.
     */
    template <typename T>
    size_t BufferManager<T>::willCompact(size_t aNewSize, [[maybe_unused]] size_t anOffset){
        bufferSize = aNewSize;
        if (aNewSize < bufferCapacity) {
            size_t newCapacity = bufferCapacity;
            while (newCapacity > aNewSize) {
                newCapacity /= 2;
            }
            if (newCapacity < bufferCapacity) {
                changeCapacity(newCapacity);
            }
        }
        return bufferCapacity;
    }

    /**
     * @brief Changes the capacity of the buffer to the specified new capacity.
     *
     * This function is a helper function used internally for resizing the buffer.
     *
     * @tparam T The type of elements stored in the buffer.
     * @param newCapacity The desired new capacity of the buffer.
     * @return void
     */
    template <typename T>
    void BufferManager<T>::changeCapacity(size_t newCapacity){
        std::unique_ptr<T[]> tempBufferPtr{std::make_unique<T[]>(newCapacity)};
        if(bufferPtr){
            std::memcpy(tempBufferPtr.get(), bufferPtr.get(), bufferSize);
        }
        bufferPtr = std::move(tempBufferPtr);
        bufferCapacity = newCapacity;
    }

} // namespace ECE141

#endif /* BufferManager_h */