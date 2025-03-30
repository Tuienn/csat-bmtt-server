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

bool FileHandler::isValidImage(const std::string& filename, const std::vector<char>& fileData) {
    // Check file size
    if (fileData.size() > MAX_FILE_SIZE) {
        std::cerr << "File too large. Maximum size is " << MAX_FILE_SIZE / (1024 * 1024) << "MB" << std::endl;
        return false;
    }

    // Check file extension
    std::filesystem::path path(filename);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool hasValidExtension = false;
    for (const auto& ext : allowedExtensions) {
        if (extension == ext) {
            hasValidExtension = true;
            break;
        }
    }

    if (!hasValidExtension) {
        std::cerr << "Invalid file type. Allowed types: PNG, JPG, JPEG" << std::endl;
        return false;
    }

    // Basic image header validation
    if (fileData.size() < 8) {
        std::cerr << "File too small to be a valid image" << std::endl;
        return false;
    }

    // Check PNG header
    if (extension == ".png" && 
        fileData[0] == 0x89 && fileData[1] == 0x50 && 
        fileData[2] == 0x4E && fileData[3] == 0x47) {
        return true;
    }
    
    // Check JPEG header
    if ((extension == ".jpg" || extension == ".jpeg") && 
        fileData[0] == 0xFF && fileData[1] == 0xD8) {
        return true;
    }

    std::cerr << "Invalid image file format" << std::endl;
    return false;
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