#include <iostream>
#include <filesystem>
#include <map>

// класс Гистограмма, реализация построения гистограммы
class Histogram {
public:
    /* конструктор класса с аргуметом шага,
       шаг - интервал в байтах  */
    Histogram(int step) {
        this->step = step;
    }
    /* сканирование каталога, метод начинает работу в заданном каталоге 
       и спускается по дереву каталогов, записывая по пути размеры 
       всех встретившихся ей файлов */
    void traverseAllDirectoryFolders() {
        try {
            // рекурсивный обход каталога
            for (const std::filesystem::path& path_tmp : std::filesystem::recursive_directory_iterator(std::filesystem::current_path())) {
                // проверка на то файл это или нет 
                if (!std::filesystem::is_directory(path_tmp)) {
                    int sizeFile = std::filesystem::file_size(path_tmp);    // размер файла
                    map[findKey(sizeFile)]++;                               // увеличить поле гистограммы на 1
                }
            }
        }
        catch (std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
        }

        // для красивого построения гистограммы
        auto it = map.end(); --it;
        maxLength = length(it->first * step) + length((it->first + 1) * step);
    }
    /* распечатать гистограмму размеров файлов, 
       используя шаг гистограммы в качестве парамет */
    void printHistogram() {
        // обход всех записей
        for (auto it = map.begin(); it != map.end(); ++it) {

            // все записи в этом блоке построения гистограммы

            std::cout << "[" << it->first * step << " - " << (it->first + 1) * step << "] ";

            int n = maxLength - (length(it->first * step) + length((it->first + 1) * step));

            while (n > 0) {
                n--;
                std::cout << " ";
            }
            std::cout << " │";

            for (int i = 0; i < it->second; i++) {
                std::cout << "▀";
            }
            std::cout << "\n";
        }
    }

private:
    /* это ассоциативный контейнер, в котором хранятся элементы, 
       образованные комбинацией значения ключа(интервал в байтах) 
       и сопоставленного значения(количество встречающихся элементов в заданном интервале) 
       в определенном порядке   */
    std::map<int, int> map;

    // шаг - интервал в байтах
    int step = 1024;
    // для красивого построения гистограммы
    int maxLength = 1;

    // определить в какой интервал входит размер файла
    int findKey(int sizeFile) {
        return sizeFile / step;
    }
    // для красивого построения гистограммы
    int length(int x) {
        if (x == 0) return 1;
        return (log10(x) + 1);
    }
};

int main() {
    Histogram histogram(1024);
    histogram.traverseAllDirectoryFolders();
    histogram.printHistogram();
    return 0;
}