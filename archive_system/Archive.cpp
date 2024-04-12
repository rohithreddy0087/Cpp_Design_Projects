//
//  Archive.cpp
//
//
//
//
#include <iomanip>
#include <filesystem>
#include <cstring>
#include "Archive.hpp"

namespace ECE141 {

    auto createFileMode = std::fstream::binary | std::fstream::trunc | std::fstream::in | std::fstream::out;
    auto openFileMode = std::fstream::binary | std::fstream::in | std::fstream::out;

    void ArchiveObserver::operator()(ActionType anAction, const std::string &aName, bool status) {
        std::string actionStr;
        switch(anAction) {
            case ActionType::added: actionStr = "added"; break;
            case ActionType::extracted: actionStr = "extracted"; break;
            case ActionType::removed: actionStr = "removed"; break;
            case ActionType::listed: actionStr = "listed"; break;
            case ActionType::dumped: actionStr = "dumped"; break;
            case ActionType::compacted: actionStr = "compacted"; break;
            default: actionStr = "unknown action"; break;
        }
        std::cout << "Document \"" << aName << "\" was " << actionStr
                  << ". Operation success: " << (status ? "true" : "false") << std::endl;
    }

    Archive &Archive::addObserver(std::shared_ptr<ArchiveObserver> anObserver) {
        observers.push_back(anObserver);
        return *this;
    }

    // Archive Class
    Archive::Archive(const std::string &aFullPath, CreateFile aMode): fullPath(aFullPath), stream(aFullPath, createFileMode) {
        writeBlockToStream(firstBlock, 0);
    }

    Archive::Archive(const std::string &aFullPath, OpenFile aMode): fullPath(aFullPath), stream(aFullPath, openFileMode) {
        readBlockFromStream(firstBlock, 0);
        addDetails(firstBlock.header.fileName, firstBlock.header.occupiedSpace, firstBlock.header.timeStamp, firstBlock.header.occupied, firstBlock.header.processed);
        for(int index = 1; index < getTotalBlockInStream(); index++) {
            Block aBlock;
            readBlockFromStream(aBlock, index);
            addDetails(aBlock.header.fileName, aBlock.header.occupiedSpace, aBlock.header.timeStamp, aBlock.header.occupied, aBlock.header.processed);
        }
    }

    Archive::~Archive(){
        stream.flush();
        stream.close();
    }

    size_t Archive::getTotalBlockInStream() {
        stream.clear();
        stream.seekg(0, std::ios::end);
        return static_cast<size_t>(stream.tellg() / sizeof(Block)); // Returns the total number of blocks in archive
    }

    // Visitor with a callback, used to read and write blocks
    bool Archive::each(BlockVisitor aVisitor) {
        size_t theCount=getTotalBlockInStream(), i{0};
        bool   more=theCount;
        Block  theBlock;

        while(more && theCount>i) {
            readBlockFromStream(theBlock,i);
            more=aVisitor(theBlock,i++);
        }
        return more;
    }

    bool Archive::getEmptyBlocks(size_t aCount, std::vector<size_t> &aList) {
        //Iterates over the vector of blocks and returns the position of first unoccupied block
        aList.clear();
        each([&](Block &aBlock, size_t aPos) { //naive approach...
            if(!aBlock.header.occupied) {
                aList.push_back(aPos);
            }
            return (aList.size()==aCount); //quit once we have aCount items...
        });

        size_t theBCount=getTotalBlockInStream();
        while(aList.size()<aCount) {
            aList.push_back(theBCount++);
        }
        aList.push_back(0);

        return true;
    }

    void Archive::addFile(const std::string& fileName, const size_t fileSize, const std::vector<uint8_t>& fileData, const bool processed) {
        size_t thePos{0};
        size_t len{fileSize};
        size_t offset{0}; // Offset to keep track of the position in the fileData vector
        std::vector<size_t> thePlaces;
        size_t num_chunks = (fileSize/kBlockDataSize)+(fileSize % kBlockDataSize ? 1: 0);
        getEmptyBlocks(num_chunks, thePlaces);

        while (len) {
            Block theBlock;
            size_t delta = std::min(kBlockDataSize, len);
            len -= delta;
            updateBlock(theBlock, true, delta, thePos, thePlaces[thePos+1],fileName, getCurrentDate(), processed);
            // Copy the appropriate part of fileData to the block data
            std::memcpy(theBlock.blockData, fileData.data() + offset, delta);
            writeBlockToStream(theBlock, thePlaces[thePos]);
            offset += delta;
            thePos++;
        }
    }

    // Writes a block to stream given a position
    bool Archive::writeBlockToStream(Block &aBlock, size_t aBlockNum) {
        stream.seekp(aBlockNum*kBlockSize, std::ios::beg);
        stream.write (reinterpret_cast<char*>(&aBlock), kBlockSize);
        stream.flush();
        return true;
    }

    // Reads a block from stream and pushes it into vector
    bool Archive::readBlockFromStream(Block &aBlock, size_t aBlockNum) {
        stream.seekg(aBlockNum * kBlockSize);
        stream.read(reinterpret_cast<char*>(&aBlock), kBlockSize);
        return true;
    }

    // Creates a new archive file, if file is already present it removes data
    ArchiveStatus<std::shared_ptr<Archive>> Archive::createArchive(const std::string &anArchiveName) {
        std::string fileName = addExtension(anArchiveName);
        auto archive = std::shared_ptr<Archive>(new Archive(fileName, CreateFile{}));
        if (!archive->stream.is_open()) {
            return ArchiveStatus<std::shared_ptr<Archive>>(ArchiveErrors::fileOpenError);
        }
        return ArchiveStatus<std::shared_ptr<Archive>>(archive);
    }

    // Opens an existing archive file
    ArchiveStatus<std::shared_ptr<Archive>> Archive::openArchive(const std::string &anArchiveName) {
        std::string fileName = addExtension(anArchiveName);
        auto archive = std::shared_ptr<Archive>(new Archive(fileName, OpenFile{}));
        if (!archive->stream.is_open()) {
            return ArchiveStatus<std::shared_ptr<Archive>>(ArchiveErrors::fileOpenError);
        }
        return ArchiveStatus<std::shared_ptr<Archive>>(archive);
    }

    ArchiveStatus<bool> Archive::add(const std::string &aFilename, IDataProcessor* aProcessor) {
        std::string fileName = extractFilename(aFilename);
        auto it = fileDetails.count(fileName);
        if (it > 0) { // If file is already present, returns fileExists error
            return ArchiveStatus<bool>(ArchiveErrors::fileExists);
        }
//        processors.push_back(static_cast<const std::shared_ptr<IDataProcessor>>(aProcessor));
        std::fstream theStream(aFilename, openFileMode);
        theStream.seekg(0, std::ios::end);
        size_t fileSize = theStream.tellg(); // Gets the file size in bytes

        std::vector<uint8_t> fileData(fileSize);
        theStream.seekg(0, std::ios::beg);
        theStream.read(reinterpret_cast<char*>(fileData.data()), fileSize);

        std::vector<uint8_t> processedData;
        bool processed = false;
        if (aProcessor) {
            processedData = aProcessor->process(fileData); // Process file data
            processed = true;
        } else {
            // If no processor provided, just use the raw file data
            processedData = std::move(fileData);
        }

        addFile(fileName, processedData.size(), processedData, processed);
        addDetails(fileName, processedData.size(), getCurrentDate(), true, processed); // adds details into private member
        return ArchiveStatus<bool>(true);
    }

    ArchiveStatus<bool> Archive::extract(const std::string &aFilename, const std::string &aFullPath) {
        std::string fileName = extractFilename(aFilename);
        auto it = fileDetails.count(fileName);
        if (it == 0) { // If file is not present, returns fileNotFound error
            return ArchiveStatus<bool>(ArchiveErrors::fileNotFound);
        }

        std::fstream outputFile(aFullPath, std::ios::out | std::ios::binary); // Open output file for writing

        if (!outputFile.is_open()) { // Check if output file is opened successfully
            return ArchiveStatus<bool>(ArchiveErrors::fileOpenError);
        }


        std::vector<uint8_t> extractedData;
        // Loop through each block to extract and potentially reverse process the data
        bool isProcessed = false;
        auto fileBlockNum = searchFileInStream(fileName);
        do{
            Block block;
            readBlockFromStream(block, fileBlockNum);
//            if(block.header.fileName != fileName) continue;
            // Extract data from block
            if (block.header.processed){isProcessed = true;};
            extractedData.insert(extractedData.end(), block.blockData, block.blockData + block.header.occupiedSpace);
            fileBlockNum = block.header.aNextBlock;
        } while(fileBlockNum != 0);

        // Reverse process the data if an IDataProcessor is provided
        if( isProcessed && DeCompressor) {
            extractedData = DeCompressor->reverseProcess(extractedData);
        }

        // Write the processed or reverse-processed data to the output file
        outputFile.write(reinterpret_cast<const char*>(extractedData.data()), extractedData.size());

        outputFile.close(); // Close output file

        return ArchiveStatus<bool>(true);
    }

    ArchiveStatus<bool> Archive::remove(const std::string &aFilename) {
        std::string fileName = extractFilename(aFilename);
        auto it = fileDetails.count(fileName);
        if (it == 0) { // If file is not present, returns fileNotFound error
            return ArchiveStatus<bool>(ArchiveErrors::fileNotFound);
        }

        //Iterates each block to check if the block's file name matches and updates the block
        auto fileBlockNum = searchFileInStream(fileName);
        size_t nextBlockNum = -1;
        do{
            Block block;
            readBlockFromStream(block, fileBlockNum);
            nextBlockNum = block.header.aNextBlock;
            updateBlock(block, false, 0, 0, 0,"", "", false);
            writeBlockToStream(block, fileBlockNum);
            fileBlockNum = nextBlockNum;
        } while(fileBlockNum != 0);

        fileDetails.erase(fileName);  // Erases file from private member
        return ArchiveStatus<bool>(true);
    }


    ArchiveStatus<size_t> Archive::list(std::ostream &aStream) {
        aStream << "###  " << std::setw(12) << "name" << std::setw(12) << "size" << std::setw(22) << "date added" << "\n";
        aStream << "-----------------------------------------------------\n";;
        int i = 0;
        for (const auto& pair : fileDetails) {
            const std::vector<std::string>& document = pair.second;
            aStream << std::setw(2) << i + 1 << ".   " << std::setw(12) << pair.first << std::setw(12) << document[0] << std::setw(22) << document[1] << "\n";
            ++i;
        }
        return ArchiveStatus<size_t>(fileDetails.size());
    }

    ArchiveStatus<size_t> Archive::debugDump(std::ostream &aStream) {
        aStream << "###  " << std::setw(4) << "status" << std::setw(12) << "name"<< "\n";
        aStream << "------------------------------------\n";
        int i = 0;
        for(int index = 0; index < getTotalBlockInStream(); index++){
            Block block;
            readBlockFromStream(block, index);
            std::string flag = block.header.occupied ? "used": "empty"; // flag as used if it is occupied else empty
            std::string fileName = block.header.occupied ? block.header.fileName: ""; // fileName is empty if the lock is unoccupied
            aStream << std::setw(2) << i + 1 << ".   " << std::setw(4) << flag << std::setw(22) << fileName << "\n";
            ++i;
        }
        return ArchiveStatus<size_t>(getTotalBlockInStream());
    }

    ArchiveStatus<size_t> Archive::compact() {
        std::string tempFilePath = fullPath + ".tmp";
        std::fstream tempStream(tempFilePath, createFileMode);

        if (!tempStream.is_open()) {
            return ArchiveStatus<size_t>(ArchiveErrors::fileOpenError);
        }

        size_t totalBlocks = getTotalBlockInStream();
        size_t occupiedBlocks = 0;
        Block block;

        for (size_t i = 0; i < totalBlocks; i++) {
            readBlockFromStream(block, i);
            if (block.header.occupied) {
                // Write only occupied blocks to the temporary stream
                tempStream.write(reinterpret_cast<char*>(&block), sizeof(Block));
                ++occupiedBlocks;
            }
        }

        tempStream.flush();
        tempStream.close();

        stream.close();
        std::remove(fullPath.c_str());
        std::rename(tempFilePath.c_str(), fullPath.c_str()); // Rename the temporary file to the original archive file name

        // Re-open the original (now compacted) archive file stream
        stream.open(fullPath, std::ios::in | std::ios::out | std::ios::binary);
        if (!stream.is_open()) {
            return ArchiveStatus<size_t>(ArchiveErrors::fileOpenError); // Return an error if the archive cannot be reopened
        }

        return ArchiveStatus<size_t>(occupiedBlocks);
    }

    [[maybe_unused]] ArchiveStatus<std::string> Archive::getFullPath() const {
        return ArchiveStatus<std::string>(fullPath);
    }

    // Assigns data to Block Header
    void Archive::updateBlock(Block& theBlock, bool occupied, size_t occupiedSpace, size_t thePos, size_t nextBlock, const std::string &fileName,
                              const std::string &timeStamp, const bool processed) {
        std::memset(theBlock.blockData, 0, kBlockDataSize);
        theBlock.header.occupied = occupied;
        theBlock.header.aPartNum = thePos;
        theBlock.header.aNextBlock = nextBlock;
        fileName.copy(theBlock.header.fileName, sizeof(theBlock.header.fileName)-1);
        theBlock.header.fileName[fileName.size()] = '\0';
        theBlock.header.occupiedSpace = occupiedSpace;
        timeStamp.copy(theBlock.header.timeStamp, sizeof(theBlock.header.timeStamp)-1);
        theBlock.header.timeStamp[timeStamp.size()] = '\0';
        theBlock.header.processed = processed;
    }

    size_t Archive::searchFileInStream(const std::string& fileName){
        for(int i = 0; i< getTotalBlockInStream(); ++i){
            Block block;
            readBlockFromStream(block, i);
            if(block.header.fileName == fileName){
                return i;
            }
        }
        return std::numeric_limits<size_t>::max();
    }

    // Extracts filename from filepath
    std::string Archive::extractFilename(const std::string &filePath) {
        std::filesystem::path pathObj(filePath);
        std::string file = pathObj.filename().string();
        return file;
    }

    // Adds extension if not present
    std::string Archive::addExtension(const std::string &basicString) {
        std::string fileName = basicString;
        if (fileName.find(".arc") == std::string::npos) {
            fileName += ".arc";
        }
        return fileName;
    }

    // Gets Current Timestamp
    std::string Archive::getCurrentDate() {
        std::time_t now = std::time(nullptr);
        std::tm* currentTime = std::localtime(&now);
        std::stringstream ss;
        ss << std::put_time(currentTime, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    // Adds details into private member
    void Archive::addDetails(const std::string& fileName, size_t fileSize, const std::string& timeStamp, bool occupied, bool processed){
        if(!occupied) return;
        auto it = fileDetails.count(fileName);
        if (it == 0) {
            fileDetails[fileName] = std::vector<std::string>();
            fileDetails[fileName].push_back(std::to_string(fileSize));
            fileDetails[fileName].push_back(timeStamp);
            fileDetails[fileName].push_back(std::to_string(processed));
        } else{
            fileDetails[fileName][0] = std::to_string( std::stoull(fileDetails[fileName][0]) + fileSize);
            fileDetails[fileName][1] = timeStamp;
            fileDetails[fileName][2] = std::to_string(processed);
        }
    }
}