Необходимо разработать функцию трансляции из виртуального адреса в физический для архитектуры x86 на языке C.
Для успешного решения задачи необходимо ознакомится с механизмом трансляции адресов, описание которого можно
найти либо в официальном руководстве от Intel, либо в многочисленных статьях в интернете.

    

    На вход функции подаются параметры:

     - виртуальный адрес

     - вид трансляции (2 или 3 уровня)

     - физический адрес корня дерева трансляции

     - функция чтения памяти по физическому адресу (callback)

    

    На выходе функция должна вернуть физический адрес и код ошибки, который покажет успешна ли была
    трансляция или нет.

    

    API для реализации:

    

    // чтение указанного количества байт физической памяти по заданному адресу в указанный буфер

    // функция вернет количество прочианных байт (меньшее или 0 означает ошибку - выход за пределы памяти)

    typedef unsigned int (*PREAD_FUNC)(void *buf, const unsigned int size, const unsigned int physical_addr);

    

    // функция трансляции виртуального адреса в физический:

    // virt_addr - виртуальный адрес подлежащий трансляции

    // level - количество уровней трансляции 2 или 3 (соответствуют legacy и PAE трансляциям в x86)

    // root_addr - физический адрес корня дерева трансляции (соответствует регистру CR3)

    // read_func - функция для чтения физической памяти (см. объявление выше)

    // функция возвращает успешность трансляции: 0 - успешно, не 0 - ошибки

    // phys_addr - выходной оттранслированный физический адрес

    int va2pa(const unsigned int virt_addr, const unsigned int level, const unsigned int root_addr,
    const PREAD_FUNC read_func, unsigned int *phys_addr)

    

    Допускается для простоты опустить проверку прав доступа к памяти, но если это будет сделано, это будет
    дополнительным плюсом к реализации.

    Допускается опустить реализацию 64-битной трансляции, но если она будет сделана это тоже будет плюсом.

    Количество фиксируемых ошибочных ситуаций отдается на выбор программисту, чем больше их будет
    предусмотрено, тем лучше.

    

    Ответ к заданию должен быть оформлен в виде файла, содержащего реализацию требуемой функции и другой
    требуемый функционал (другие функции, если нужна декомпозиция, тесты и т.д.). Использование стандартных
    библиотек не лимитируется, хотя в данной задаче они избыточны.

