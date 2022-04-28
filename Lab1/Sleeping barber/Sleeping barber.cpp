#define _CRT_SECURE_NO_WARNINGS // для работы localtime

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <random>

/*  это примитив синхронизации, который можно использовать
    для блокировки потока или нескольких потоков одновременно,
    пока другой поток не изменит общую переменную(условие) и не
    уведомит об этом condition_variable. */
std::condition_variable cv;

/*  этот мьютекс используется для условной переменной cv    */
std::mutex m_;

/*  общая переменная для потока */
bool workBarber_ = true;

/*  класс Runnable, служит общим представлением барбера и клиента   */
class Runnable {
public:
    /*  это циклический процесс, и каждый раз,
        проходя свой цикл, он производит определенную порцию информации,
        которую должен обработать барбер  */
    void barber() {
        while(true){
            {
                std::unique_lock<std::mutex> lk(m_);

                if (customerCnt_ == 0) {
                    workBarber_ = false;    // барбер не работает
                    time_ = nowTime();      // время в данный момент

                    std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
                    std::cout << "  Time: " << time_->tm_hour << ":" << time_->tm_min
                              << ":" << time_->tm_sec << "\n";
                    std::cout << "  barber falls asleep\n\n";

                    cv.wait(lk, [] {return workBarber_; }); // спать пока клиент не разбудит
                }
            }
            {
                std::unique_lock<std::mutex> lk(m_);

                customerCnt_--;     // одним ожидающим клиентом меньше
                time_ = nowTime();  // время в данный момент

                std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
                std::cout << "  Time: " << time_->tm_hour << ":" << time_->tm_min
                          << ":" << time_->tm_sec << "\n";
                std::cout << "  The barber begins to work\n\n";

                lk.unlock();        // освободить мютекс

                /* задержка или время работы барбера    */
                int duration = generationRandomSleep(3000, 6000);
                std::this_thread::sleep_for(std::chrono::milliseconds(duration));

                time_ = nowTime();  // время в данный момент

                std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
                std::cout << "  Time: " << time_->tm_hour << ":" << time_->tm_min
                          << ":" << time_->tm_sec << "\n";
                std::cout << "  the barber ends to work\n\n";

            }
        }
    }
    /*  это циклический процесс, и каждый раз,
        проходя свой цикл, он производит определенную порцию информации,
        которую должен обработать барбер    */
    void customer() {
        while (true) {
            /* задержка или время прихода барбера    */
            int duration = generationRandomSleep(5000, 20000);
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            {
                if (customerCnt_ >= numberSeats_) { // проверка на свободные места
                    std::lock_guard<std::mutex> lk(m_);

                    time_ = nowTime();  // время в данный момент

                    std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
                    std::cout << "  Time: " << time_->tm_hour << ":" << time_->tm_min
                              << ":" << time_->tm_sec << "\n";
                    std::cout << "  customer leaves because seats are full\n\n";
                }
                else {
                    customerCnt_++;         // одним ожидающим клиентом меньше
                    if (!workBarber_) {     // проверка, спит ли барбер
                        std::lock_guard<std::mutex> lk(m_);

                        time_ = nowTime();  // время в данный момент

                        std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
                        std::cout << "  Time: " << time_->tm_hour << ":" << time_->tm_min
                                  << ":" << time_->tm_sec << "\n";
                        std::cout << "  customer woke the barber\n\n";

                        workBarber_ = true; // барбер работает
                        cv.notify_one();    // разбудить барбера
                    }
                    else {
                        std::lock_guard<std::mutex> lk(m_);

                        time_ = nowTime();  // время в данный момент

                        std::cout << "> id thread = " << std::this_thread::get_id() << "\n";
                        std::cout << "  Time: " << time_->tm_hour << ":" << time_->tm_min
                                  << ":" << time_->tm_sec << "\n";
                        std::cout << "  the client took an empty seat\n\n";
                    }
                }
            }
        }
    }
private:
    /*  время   */
    tm* time_;

    /*  количество ожидающих клиентов   */
    int customerCnt_ = 0;

    /*  количество мест   */
    int numberSeats_ = 2;

    /*  генерация случайного числа задержки

        @param min минимальное число с какого рандомится число
        @param max максимальное число до какого рандомится число

        @returns случайное число задержки   */
    int generationRandomSleep(int min, int max) {
        static std::mt19937 rnd(std::time(nullptr));
        return std::uniform_int_distribution<>(min, max)(rnd);
    }

    /* фунція для знаходження точного часу */
    tm* nowTime()
    {
        time_t now = time(NULL);
        struct tm* tm_struct = localtime(&now);
        return tm_struct;
    }
};

int main() {
    Runnable runnable;

    std::thread t1(&Runnable::barber, &runnable);
    std::thread t2(&Runnable::customer, &runnable);
    std::thread t3(&Runnable::customer, &runnable);
    std::thread t4(&Runnable::customer, &runnable);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}