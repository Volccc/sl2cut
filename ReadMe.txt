sl2cut - "резалка" sl2 файлов
Запуск из командной строки:
sl2cut file.sl2 - просто выводит информацию что есть в файле
ключи программы:
-c start:finish - с какого и по какой кадр выводить
-d [PDS] - какие каналы убрать. Возможные буквы:
 P - уберутся с обычного датчика, каналы Primary Secondary
 D - уберется канал DSI
 S - уберутся каналы SideScan - Left, Right, Composite
-a - Координаты начала лога (и дальше пропорционально) подменяются на заданные в программе
-n - Задать широту для начальной точки лога
-e - Задать долготу для начальной точки лога
Имя выходного файла получается из исходного добавлением диапазона

Пример:
sl2cut -c 2000:10000 -n 46.45 -e 6.5 -d PD sonar01.sl2
В файл sonar01_2000-1000.sl2 выведутся кадры с 2000 по 10000, обычный сонар и DSI удалятся, останется только SideScan. Лог будет в Женевском озере (эти значения координат используются и с ключом -a)
Найти нужные номера кадров можно с помощью Sonar Log Viewer от Lowrance.
=============
sl2cut - console application to cut .sl2 files
Command:
sl2cut file.sl2 - only print info about this log file
Options:
-c start:finish start and end sounding to output. You may find it in Lowrance Sonar Log Viewer
-d [PDS] - which channels to delete
 P - remove primary/secondary channels
 D - remove DSI channel
 S - remove SideScan channels: Left, Right, Composite
-a - anonymize log: output log coordinates will be shifted with constants (hardcoded in utility)
-n - define northing shift
-e - define easting shift
Output file name is input name + _start_end
Example:
sl2cut -c 2000:10000 -n 46.45 -e 6.5 -d PD sonar01.sl2
Output file sonar01_2000-1000.sl2 will contain sounding from 2000 to 10000, primary and DSI channels will be removed (only Sidescan in output). Track will start in Geneva lake.
