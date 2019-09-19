//
//  main.cpp
//  MultithreadingVSULab1
//
//  Created by Artemy Cheprakov on 19/09/2019.
//  Copyright © 2019 VSU. All rights reserved.
//

#include <iostream>
#include <thread> // директива, отвечающая за потоки
#include <vector>
#include <string>
#include <mutex>

const int VEC_SIZE = 10; // размер массива
const short int SCHED_VAL = 3; // цифра, которую будем искать на конце чисел
const short int THREADS_AMOUNT = 2; // количество потоков, не использовал, но мб когда-то пригодится

std::recursive_mutex _lock;
bool flag = false; // флаг, сигнализирующий о нахождении числа

template<typename T>
/* дженерик: я их тут использую, чтобы программа работала как для целых чисел,
 так и для вещественных */
void fillVector(std::vector<T> &vec) { // функция, которая заполняет массив
    
    switch( *typeid(T).name() ) { // смотрим на то, какого типа массив мы передали
        case 'i': // если числа интегер, то заполняем массив ЦЕЛЫМИ числами от 1 до ста
            for (int i = 0; i < VEC_SIZE; ++i)
                vec.push_back(rand() % 101);
            break; // и выходим
        case 'd': case 'f': // если дабл или флоат, то заполняем вещественными
            for (int i = 0; i < VEC_SIZE; ++i) {
                vec.push_back((rand() % 10000) / 100.0);
                /* генерирую число 0-9999, потом делю на 100,как раз и получаются вещественные числа 0-99.99 */
            }
            break; // и выходим
        default: // если не инт не дабл не флоат, то кидаем ошибку выполнения программы
            throw std::runtime_error("incorrect mass type");
    }
}

template <typename T> // такой же дженерик
short int getLastDigit(T var) {
    /* функция принимает на вход один элемент массива, и, в зависимости от его типа, возвращает последнюю цифру,
     для целых просто последнюю, для вещественных последнюю в дробной части */

    switch( *typeid(T).name() ) { // смотрим на тип
        case 'i': // если целые
            return static_cast<int>(var) % 10; // просто вернем остаток от деления на 10
        case 'd': case 'f': // вещественные
            while(var - static_cast<int>(var) != 0.0)
                var *= 10; // умножаем на 10, пока не избавимся от дробной части
            return static_cast<int>(var) % 10; // возвращаем остаток от деления на 10
        default:
            // сюда мы никогда не зайдем, но на всякий если не инт не дабл не флоат вернуть '-1' как невозможный исход
            return -1;
    }
}

template <typename T> // дженерик
void searchForLastDigitInMass(const std::vector<T> &vec, unsigned int beginIndex, unsigned int endIndex) {
    /* на вход принимаем массив, флаг, который будет индикатором того, что нужное число было найдено, индексы начала и конца массива*/
                                          
    short int var; // тут будем хранить последнюю цифру числа
    
    std::cout << "Thread " << std::this_thread::get_id() << " here!\n";
    /* строка выше - индикатор того, что потоки с разными айди заходят в эту функцию, которая называется
     функцией потока, т.к каждый поток выполняет именно ее. То, что в серидине сиаута это метод получения
     айди потока*/
    
    for (unsigned int i = beginIndex; i < endIndex; ++i) {
        
        var = getLastDigit(vec[i]);
        /* вызывем функцию поиска последнего числа, пока не найдем такое, что нам подходит,
         или не дойдем до конца куска массива, за который ответственен данынй поток */
        
        if (var == SCHED_VAL) {
            std::lock_guard<std::recursive_mutex> locker(_lock);
            flag = true;
            break;
        } else continue;
    }
}

template <typename T>
void showVec(const std::vector<T> &vec) { // вывод массива

    for (int i = 0; i < vec.size(); ++i)
        std::cout << vec[i] << " ";
    
    std::cout << std::endl;
}

int main() {
    
    srand((unsigned int)time(NULL)); // для генерации рандомных чисел
    
    std::vector<double> vec; // массив, можно вместо дабл писать инт или флоат, все будет работать (наверно)
    
    try { // пытаемся заполнить массив
        fillVector<>(vec);
    } catch (std::string ex) { // если массив определен неправильно, ловим ошибку выполнения
        std::cout << ex << std::endl; // выводим сообщение об ошибке
        return -1; // завершаем работу программы
    }

    /* ниже это инициализация потоков, у меня пока 2. В качестве аргументов конструктора передаются сначала ссылка на функцию,
     с которой работает поток, потом все параметры функции через запятую, если менять дабл на что-то выше в коде, то переписывать и тут.
     Все значения, что передаются по ссылке, должны быть обернуты в std::ref, также для 1-го потока я передаю первую половину массива,
     для 2-го - вторую, пока не придумал, как быть с некратными размерами*/
    std::thread t1(searchForLastDigitInMass<double>, vec, 0, vec.size() / 2);
    
    std::thread t2(searchForLastDigitInMass<double>, vec, vec.size() / 2, vec.size());
    
    t1.join();
    t2.join();
    
    showVec<>(vec); // вывожу массив для проверки работы программы
    
    std::cout << flag << std::endl; // вывожу флаг
    
    return 0;
    
    /* чтобы проверить работу в однопоточном режиме, можно просто вызвать функцию searchForLastDigitInMass указав начало массива на 0,
     а конец - на последний элемент */
}
