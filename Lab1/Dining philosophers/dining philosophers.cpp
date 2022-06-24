#define _CRT_SECURE_NO_WARNINGS // для работы localtime

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>
#include <random>

// Количество философов
const int N = 5;

// Номер левого соседа id
#define LEFT (id + N - 1) % N

// Номер правого соседа id
#define RIGHT (id + 1) % N

// класс философа, реализация всех активностей философа
class Philosopher {
public:
    enum {
        THINKING = 0,   // философ думает
        EATING = 2      // философ кушает
    };
    /* это циклический процесс, и каждый раз,
       проходя свой цикл, он производит определенную порцию информации,
       которую должен обработать философ */
    void philosophicalProcess(int id) {
        while (true) {
            think(id);      // думать
            takeForks(id);  // взять вилки
            eat(id);        // кушать
            put_forks(id);  // положить вилки
        }
    }
    // заполнить массив семафоров значениями
    void populateArraySemaphores() {
        for (int i = 0; i < N; i++) {
            s[i] = new std::binary_semaphore{ 0 };
        }
    }
private:
    std::mutex m_;
    std::mutex mutex_;
    // время 
    tm* time_;

    // массив состояний семафоров
    std::binary_semaphore* s[N];

    // массив состояний философов
    int state[5] = { 0, 0, 0, 0, 0 };
    
    // вектор имен философов
    std::vector<std::string> names_{ "ARISTOTLE", "SOCRATES", "PLATO", "CONFUCIUS", "DEVTEROV" };

    /*  генерация случайного числа задержки
        @param min минимальное число с какого рандомится число
        @param max максимальное число до какого рандомится число
        @returns случайное число задержки   */
    int generationRandomSleep(int min, int max) {
        static std::mt19937 rnd(std::time(nullptr));
        return std::uniform_int_distribution<>(min, max)(rnd);
    }

    /* фунція для знаходження точного часу */
    tm* nowTime() {
        time_t now = time(NULL);
        struct tm* tm_struct = localtime(&now);
        return tm_struct;
    }
    // функция проверки соседей едят они или нет
    void check(int id) {
        if (state[LEFT] != EATING && state[RIGHT] != EATING) {
            state[id] = EATING;
            s[id]->release();
        }
    }
    // осуществление мыслительного процесса философа
    void think(int id) {
        {
            std::lock_guard<std::mutex> lk(m_);

            time_ = nowTime();  // время в данный момент

            std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
            std::cout << " Time: " << time_->tm_hour << ":" << time_->tm_min
                      << ":" << time_->tm_sec << "\n";
            std::cout << " The " << names_[id] << " (" << id + 1<< ") begins to think\n\n";
        }

        // задержка или время работы барбера
        int duration = generationRandomSleep(4000, 10000);
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));

        time_ = nowTime();      // время в данный момент

        std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
        std::cout << " Time: " << time_->tm_hour << ":" << time_->tm_min
                  << ":" << time_->tm_sec << "\n";
        std::cout << " The " << names_[id] << " (" << id + 1 << ") ends to think\n\n";
    }
    // процесс взять вилки
    void takeForks(int id) {
        mutex_.lock();
        check(id);              // проверить философа, на возможнось приступить кушать
        mutex_.unlock();
        s[id]->acquire();
    }
    // функция обеда
    void eat(int id) {
        {
            std::lock_guard<std::mutex> lk(m_);

            time_ = nowTime();  // время в данный момент

            std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
            std::cout << " Time: " << time_->tm_hour << ":" << time_->tm_min
                      << ":" << time_->tm_sec << "\n";
            std::cout << " The " << names_[id] << " (" << id + 1 << ") begins to eat\n\n";
        }

        // задержка или время работы барбера
        int duration = generationRandomSleep(5000, 10000);
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));

        time_ = nowTime();      // время в данный момент

        std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
        std::cout << " Time: " << time_->tm_hour << ":" << time_->tm_min
                  << ":" << time_->tm_sec << "\n";
        std::cout << " The " << names_[id] << " (" << id + 1 << ") ends to eat\n\n";
    }
    // процесс положить вилки
    void put_forks(int id) {
        mutex_.lock();
        state[id] = THINKING;   // статус философа поменять на думать
        check(LEFT);            // проверить левого сосоеда, на возможнось приступить кушать
        check(RIGHT);           // проверить левого сосоеда, на возможнось приступить кушать
        mutex_.unlock();
    }
};

int main() {

    Philosopher philosopher;

    philosopher.populateArraySemaphores();

    std::thread t1(&Philosopher::philosophicalProcess, &philosopher, 0);
    std::thread t2(&Philosopher::philosophicalProcess, &philosopher, 1);
    std::thread t3(&Philosopher::philosophicalProcess, &philosopher, 2);
    std::thread t4(&Philosopher::philosophicalProcess, &philosopher, 3);
    std::thread t5(&Philosopher::philosophicalProcess, &philosopher, 4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}