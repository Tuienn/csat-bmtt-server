#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <string>
#include <vector>
#include <filesystem>

class FileHandler {
public:
    FileHandler();
    ~FileHandler();

    // Save uploaded file to the test directory
    bool saveFile(const std::string& filename, const std::vector<char>& fileData);
    
    // Get the upload directory path
    std::string getUploadDirectory() const;
    
    // Create upload directory if it doesn't exist
    bool createUploadDirectory();

private:
    std::string uploadDirectory;
};

#endif // FILE_HANDLER_HPP 