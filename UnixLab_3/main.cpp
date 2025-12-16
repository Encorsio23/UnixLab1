#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <vector>
#include <openssl/sha.h>

namespace fs = std::filesystem;

std::string calculateSHA1(const fs::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    SHA_CTX shaContext;
    SHA1_Init(&shaContext);

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        SHA1_Update(&shaContext, buffer, file.gcount());
    }
    SHA1_Update(&shaContext, buffer, file.gcount());

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &shaContext);

    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    file.close();
    return ss.str();
}

bool isHardLink(const fs::path& path) {
    try {
        return fs::hard_link_count(path) > 1;
    }
    catch (...) {
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Использование: " << (argv[0] ? argv[0] : "hardlinks") << " <директория>\n";
        return 1;
    }

    fs::path directory(argv[1]);

    if (!fs::exists(directory)) {
        std::cerr << "Ошибка: путь не существует - " << directory << std::endl;
        return 1;
    }

    if (!fs::is_directory(directory)) {
        std::cerr << "Ошибка: указанный путь не является директорией - " << directory << std::endl;
        return 1;
    }

    std::unordered_map<std::string, fs::path> hashToOriginalFile;
    std::vector<fs::path> files;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (fs::is_regular_file(entry) &&
                !fs::is_symlink(entry) &&
                !isHardLink(entry.path())) {
                files.push_back(entry.path());
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка доступа к файлам: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Найдено " << files.size() << " файлов для обработки." << std::endl;

    int processed = 0;
    int duplicatesFound = 0;
    int linksCreated = 0;
    int errors = 0;

    for (const auto& filepath : files) {
        processed++;

        if (processed % 100 == 0) {
            std::cout << "Обработано " << processed << " из " << files.size() << " файлов..." << std::endl;
        }

        std::string hash = calculateSHA1(filepath);
        if (hash.empty()) {
            std::cerr << "Не удалось вычислить хэш для файла: " << filepath << std::endl;
            errors++;
            continue;
        }

        if (hashToOriginalFile.count(hash) > 0) {
            duplicatesFound++;
            fs::path originalFile = hashToOriginalFile[hash];

            try {
                if (fs::status(filepath).device() != fs::status(originalFile).device()) {
                    std::cerr << "Файлы находятся в разных файловых системах: " << filepath << " и " << originalFile << std::endl;
                    errors++;
                    continue;
                }
            }
            catch (...) {
                std::cout << "Внимание: не удалось проверить файловую систему для " << filepath << std::endl;
            }

            std::cout << "Дубликат " << duplicatesFound << ": " << filepath << " → " << originalFile << std::endl;

            try {
                fs::remove(filepath);
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Ошибка удаления файла " << filepath << ": " << e.what() << std::endl;
                errors++;
                continue;
            }

            try {
                fs::create_hard_link(originalFile, filepath);
                linksCreated++;
                std::cout << "Создана жёсткая ссылка: " << filepath << std::endl;
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Ошибка создания жёсткой ссылки для " << filepath << ": " << e.what() << std::endl;
                errors++;
            }
        }
        else {
            hashToOriginalFile[hash] = filepath;
        }
    }

    std::cout << "\nОбработано файлов: " << processed << std::endl;
    std::cout << "Уникальных файлов: " << hashToOriginalFile.size() << std::endl;
    std::cout << "Найдено дубликатов: " << duplicatesFound << std::endl;
    std::cout << "Создано жёстких ссылок: " << linksCreated << std::endl;
    std::cout << "Ошибок: " << errors << std::endl;

    return 0;
}