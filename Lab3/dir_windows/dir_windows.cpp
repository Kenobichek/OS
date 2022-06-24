#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <filesystem>

using namespace std::chrono_literals;

// для того, щоб бачити дату останньої зміни файла
template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

// для того, щоб бачити який доступ є у файла
void demo_perms(std::filesystem::perms p)
{
    std::cout << ((p & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? "r" : "-")
        << ((p & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? "w" : "-")
        << ((p & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? "x" : "-")
        << ((p & std::filesystem::perms::group_read) != std::filesystem::perms::none ? "r" : "-")
        << ((p & std::filesystem::perms::group_write) != std::filesystem::perms::none ? "w" : "-")
        << ((p & std::filesystem::perms::group_exec) != std::filesystem::perms::none ? "x" : "-")
        << ((p & std::filesystem::perms::others_read) != std::filesystem::perms::none ? "r" : "-")
        << ((p & std::filesystem::perms::others_write) != std::filesystem::perms::none ? "w" : "-")
        << ((p & std::filesystem::perms::others_exec) != std::filesystem::perms::none ? "x" : "-")
        << '\n';
}

// власна версія програми dir 
void dir(std::filesystem::path path) {
    try {
        // рекурсивный обход каталога
        for (const std::filesystem::path& path_tmp : std::filesystem::directory_iterator(path)) {

            // інформація про ім'я файла
            std::cout << "name: " << path_tmp.filename() << "\n";

            // інформація про розмір файла
            std::cout << "size: " << std::filesystem::file_size(path_tmp) << "\n";
            
            // інформація про тип файла
            std::cout << "extension: " << path_tmp.extension() << "\n";

            // інформація про права доступу файла
            std::cout << "access: "; demo_perms(std::filesystem::status(path_tmp).permissions());

            // інформація про час доступу
            auto ftime = std::filesystem::last_write_time(path_tmp);
            std::time_t cftime = to_time_t(ftime);
            std::cout << "File write time is " << std::asctime(std::localtime(&cftime)) << "\n\n";
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
    }

}

int main() {
    dir("C:/Users/lesec/Desktop/student/OS/Lab3");
}