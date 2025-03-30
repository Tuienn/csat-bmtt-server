#include "file_handler.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>

FileHandler::FileHandler() {
    // Set the upload directory to "test" in the current directory
    uploadDirectory = "test";
}

FileHandler::~FileHandler() {
}

bool FileHandler::createUploadDirectory() {
    try {
        if (!std::filesystem::exists(uploadDirectory)) {
            std::filesystem::create_directory(uploadDirectory);
            std::cout << "Created upload directory: " << uploadDirectory << std::endl;
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating upload directory: " << e.what() << std::endl;
        return false;
    }
}

bool FileHandler::saveFile(const std::string& filename, const std::vector<char>& fileData) {
    try {
        // Check file size
        if (fileData.size() > MAX_FILE_SIZE) {
            std::cerr << "File too large. Maximum size is " << MAX_FILE_SIZE / (1024 * 1024) << "MB" << std::endl;
            return false;
        }

        // Ensure the upload directory exists
        if (!createUploadDirectory()) {
            return false;
        }

        // Create the full file path by converting strings to filesystem::path
        std::filesystem::path dirPath(uploadDirectory);
        std::filesystem::path filePath = dirPath / filename;

        // Write the file
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file for writing: " << filePath << std::endl;
            return false;
        }

        file.write(fileData.data(), fileData.size());
        file.close();
        
        std::cout << "Successfully saved file: " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving file: " << e.what() << std::endl;
        return false;
    }
}

std::string FileHandler::getUploadDirectory() const {
    return uploadDirectory;
} 