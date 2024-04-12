//
//  Archive.hpp
//
//
//
//

#ifndef Archive_hpp
#define Archive_hpp

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <cstring>
#include <cstdint>
#include <zlib.h>

namespace ECE141 {

    enum class ActionType {added, extracted, removed, listed, dumped, compacted};
    enum class AccessMode {AsNew, AsExisting};

    struct ArchiveObserver {
        void operator()(ActionType anAction,
                        const std::string &aName, bool status);
    };

    class IDataProcessor {
    public:
        virtual std::vector<uint8_t> process(const std::vector<uint8_t>& input) = 0;
        virtual std::vector<uint8_t> reverseProcess(const std::vector<uint8_t>& input) = 0;
        virtual ~IDataProcessor(){};
    };

    class Compression : public IDataProcessor {
    public:
        std::vector<uint8_t> process(const std::vector<uint8_t>& input) override {
            uLongf compressedSize = compressBound(input.size());
            std::vector<uint8_t> compressedData(compressedSize);

            int result = compress(compressedData.data(), &compressedSize, input.data(), input.size());

            if (result != Z_OK) {
                throw std::runtime_error("Compression failed");
            }

            compressedData.resize(compressedSize);
            return compressedData;
        }

        std::vector<uint8_t> reverseProcess(const std::vector<uint8_t>& input) override {
            std::vector<uint8_t> decompressedData(input.size() * 2); // Initial guess, may need adjustment
            uLongf decompressedSize = decompressedData.size();
            int result;

            // Inflate might need more space than our initial guess
            do {
                result = uncompress(decompressedData.data(), &decompressedSize, input.data(), input.size());
                if (result == Z_BUF_ERROR) { // Output buffer too small
                    decompressedData.resize(decompressedData.size() * 2); // Double the buffer size
                    decompressedSize = decompressedData.size();
                } else if (result != Z_OK) {
                    throw std::runtime_error("Decompression failed");
                }
            } while (result == Z_BUF_ERROR);

            decompressedData.resize(decompressedSize);
            return decompressedData;
        }
        ~Compression() override = default;
    };

    enum class ArchiveErrors {
        noError=0,
        fileNotFound=1, fileExists, fileOpenError, fileReadError, fileWriteError, fileCloseError,
        fileSeekError, fileTellError, fileError, badFilename, badPath, badData, badBlock, badArchive,
        badAction, badMode, badProcessor, badBlockType, badBlockCount, badBlockIndex, badBlockData,
        badBlockHash, badBlockNumber, badBlockLength, badBlockDataLength, badBlockTypeLength
    };

    template<typename T>
    class ArchiveStatus {
    public:
        // Constructor for success case
        explicit ArchiveStatus(const T value)
                : value(value), error(ArchiveErrors::noError) {}

        // Constructor for error case
        explicit ArchiveStatus(ArchiveErrors anError)
                : value(std::nullopt), error(anError) {
            if (anError == ArchiveErrors::noError) {
                throw std::logic_error("Cannot use noError with error constructor");
            }
        }

        // Deleted copy constructor and copy assignment to make ArchiveStatus move-only
        ArchiveStatus(const ArchiveStatus&) = delete;
        ArchiveStatus& operator=(const ArchiveStatus&) = delete;

        // Default move constructor and move assignment
        ArchiveStatus(ArchiveStatus&&) noexcept = default;
        ArchiveStatus& operator=(ArchiveStatus&&) noexcept = default;

        T getValue() const {
            if (!isOK()) {
                throw std::runtime_error("Operation failed with error");
            }
            return *value;
        }

        bool isOK() const { return error == ArchiveErrors::noError && value.has_value(); }
        ArchiveErrors getError() const { return error; }

    private:
        std::optional<T> value;
        ArchiveErrors error;
    };


    class BlockHeader{
    public:
        bool occupied{false}; // indicates whether the block is occupied or not
        bool processed{false}; // indicates whether the block is processed by compression
        size_t occupiedSpace{0}; // indicates the amount of space occupied in the block
        size_t aPartNum{1}; // indicates the number of that respective file
        size_t aNextBlock{0}; // indicates
        char fileName[40]{}; // file name stored in the block, shouldn't exceed 32 characters
        char timeStamp[20]{}; // stores the timestamp when the block is created
    };

    const size_t kBlockSize = 1024;
    const size_t kHeaderSize = sizeof(BlockHeader);
    const size_t kBlockDataSize = kBlockSize - kHeaderSize;

    class Block{
    public:
        BlockHeader header; //header of the block
        char blockData[kBlockDataSize]{}; // data to store
    };

    struct CreateFile { //auto-generate proper mode flags to create
        explicit operator std::ios_base::openmode() {
            return std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc;
        }
    };

    struct OpenFile { //auto-generate correct mode flags to open
        explicit operator std::ios_base::openmode() {
            return std::ios::binary | std::ios::in | std::ios::out;
        }
    };

    using BlockVisitor = std::function<bool(Block &aBlock, size_t aPos)>;

    class Archive {
    protected:
        std::vector<std::shared_ptr<IDataProcessor>> processors;
        std::vector<std::shared_ptr<ArchiveObserver>> observers;
        Archive(const std::string &aFullPath, CreateFile aMode);  //protected on purpose
        Archive(const std::string &aFullPath, OpenFile aMode);

        // Gets the total number of blocks in archive
        size_t getTotalBlockInStream();

        // Gets the position of the empty blocks
        bool getEmptyBlocks(size_t aCount, std::vector<size_t> &aList);

        // visitor pattern
        bool each(BlockVisitor aVisitor);

        void addFile(const std::string& fileName, const size_t fileSize, const std::vector<uint8_t>& fileData, const bool processed);

        // writes a given block into archive stream
        bool writeBlockToStream(Block &aBlock, size_t aBlockNum);

        // reads a block from archive stream
        bool readBlockFromStream(Block &aBlock, size_t aBlockNum);

    public:

        ~Archive();  //

        static    ArchiveStatus<std::shared_ptr<Archive>> createArchive(const std::string &anArchiveName);
        static    ArchiveStatus<std::shared_ptr<Archive>> openArchive(const std::string &anArchiveName);

        Archive&  addObserver(std::shared_ptr<ArchiveObserver> anObserver);

        ArchiveStatus<bool>      add(const std::string &aFilename, IDataProcessor* aProcessor=nullptr);
        ArchiveStatus<bool>      extract(const std::string &aFilename, const std::string &aFullPath);
        ArchiveStatus<bool>      remove(const std::string &aFilename);
        ArchiveStatus<size_t>    list(std::ostream &aStream);
        ArchiveStatus<size_t>    debugDump(std::ostream &aStream);

        ArchiveStatus<size_t>    compact();

        [[maybe_unused]] ArchiveStatus<std::string> getFullPath() const; //get archive path (including .arc extension)

    private:
        std::string fullPath;
        std::fstream stream;
        Block firstBlock;
        IDataProcessor* DeCompressor = new Compression();

        // Private members to store file details
        std::unordered_map<std::string, std::vector<std::string>> fileDetails;

        // Helper functions to process data into archive
        void updateBlock(Block& theBlock, bool occupied, size_t occupiedSpace, size_t thePos, size_t nextBlock, const std::string& fileName,  const std::string& timeStamp = "", const bool processed = false);
        size_t searchFileInStream(const std::string& fileName);
        void addDetails(const std::string& fileName, size_t fileSize, const std::string& timeStamp, bool occupied, bool processed);
        std::string extractFilename(const std::string &basicString);
        static std::string addExtension(const std::string &basicString);
        std::string getCurrentDate();
    };

}

#endif /* Archive_hpp */
