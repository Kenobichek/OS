#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <random>

/*  Класс BoundedBuffer, это ограниченный буфер   */
class BoundedBuffer {
public:
    /*  положить элемент в буфер    */
    void push() {
        if (size_ < capacity_) {
            size_++;
            std::cout << "> Push\t\tSize = "<< size_ << "\n\n";
        }
        else {
            std::cout << "> Buffer full!!!\n\n";
        }
    }

    /*  удалить элемент из буфера   */
    void remove() {
        if (size_ > 0) {
            size_--;
            std::cout << "> Remove\tSize = " << size_ << "\n\n";
        }
        else {
            std::cout << "> Buffer is empty!!!\n\n";
        }
    }
private:
    /*  текущий размер буфера   */
    int size_ = 0;
    /*  ограниченный размер буфера  */
    int capacity_ = 10;
};

/*  Класс Runnable, служит общим представлением производителя и потребителя   */
class Runnable {
public:
    /*  это циклический процесс, и каждый раз, 
        проходя свой цикл, он производит определенную порцию информации, 
        которую должен обработать потребитель   */
    void producer() {
        while (true) {
            int duration = generationRandomSleep(4000, 8000);
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            {
                std::lock_guard<std::mutex> g(buffer_manipulation_);
                std::cout << "id thread = " << std::this_thread::get_id() << "\n";
                buffer_.push();
            }
        }
    }

    /*  является циклическим процессом, и каждый раз, 
        когда он проходит свой цикл, он может обрабатывать очередную порцию информации, 
        как это было произведено производителем     */
    void consumer() {
        while (true) {
            int duration = generationRandomSleep(3000, 6000);
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            {
                std::lock_guard<std::mutex> g(buffer_manipulation_);
                std::cout << "id thread = " << std::this_thread::get_id() << "\n";
                buffer_.remove();
            }
        }
    }

private:
    /* ограниченный буфер */
    BoundedBuffer buffer_;
    std::mutex buffer_manipulation_;

    /*  генерация случайного числа задержки
    
        @param min минимальное число с какого рандомится число
        @param max максимальное число до какого рандомится число

        @returns случайное число задержки   */
    int generationRandomSleep(int min, int max) {
        static std::mt19937 rnd(std::time(nullptr));
        return std::uniform_int_distribution<>(min, max)(rnd);
    }
};

int main() {
    Runnable runnable;
    std::thread t1(&Runnable::producer, &runnable);
    std::thread t2(&Runnable::producer, &runnable);
    std::thread t3(&Runnable::consumer, &runnable);
    t1.join();
    t2.join();
    t3.join();
    return 0;
}