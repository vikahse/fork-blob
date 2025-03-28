# !/bin/bash

# Создаем файл со случайным содержимым
# dd if=/dev/urandom of=original_5MB.bin bs=1M count=5

# # Определяем случайную стартовую позицию (0-5 МБ)
# RAND_POS=$(( RANDOM % (5 * 1024 * 1024) ))

# # Извлекаем 5 МБ начиная со случайной позиции
# dd if=original.bin of=chunk.bin bs=1 skip=$RAND_POS count=5M

# # Выбираем случайную позицию вставки (0-10 МБ)
# INSERT_POS=$(( RANDOM % (10 * 1024 * 1024) ))

# # Создаем модифицированный файл
# {
#     # Часть до места вставки
#     dd if=original.bin bs=1 count=$INSERT_POS
    
#     # Вставляемый кусок
#     cat chunk.bin
    
#     # Оставшаяся часть после вставки
#     dd if=original.bin bs=1 skip=$INSERT_POS
# } > modified.bin

# # Размеры файлов
# ls -lh original.bin modified.bin

# # Количество отличающихся байтов
# cmp -l original.bin modified.bin | wc -l



# file1="original.bin"
# file2="modified.bin"

# # Получаем размер файла (кросс-платформенный способ)
# size=$(wc -c < "$file1" | tr -d ' ')

# prev_match=0
# current_pos=0

# while (( current_pos < size )); do
#     # Читаем по 1 байту из каждого файла
#     byte1=$(dd if="$file1" bs=1 skip=$current_pos count=1 2>/dev/null | od -An -t u1)
#     byte2=$(dd if="$file2" bs=1 skip=$current_pos count=1 2>/dev/null | od -An -t u1)

#     if [ "$byte1" = "$byte2" ]; then
#         (( current_pos++ ))
#     else
#         if (( prev_match < current_pos )); then
#             echo "Match: $prev_match - $((current_pos - 1))"
#         fi
#         (( current_pos++ ))
#         prev_match=$current_pos
#     fi
# done

# if (( prev_match < current_pos )); then
#     echo "Match: $prev_match - $((current_pos - 1))"
# fi

# !/bin/bash

# file="test.bin"
# output="test_delete.bin"

# # Получаем размер файла
# file_size=$(wc -c < "$file" | tr -d ' ')
# max_pos=$((file_size - 1048576))  # 1 МБ = 1048576 байт

# if [ $max_pos -le 0 ]; then
#   echo "Файл слишком мал для удаления 1 МБ!"
#   exit 1
# fi

# # Генерируем случайную позицию
# # cut_pos=$(shuf -i 0-$max_pos -n 1)  # Linux
# cut_pos=$(jot -r 1 0 $max_pos)    # macOS

# # Вырезаем 1 МБ
# dd if="$file" bs=1 count=$cut_pos > part1.bin
# dd if="$file" bs=1 skip=$((cut_pos + 2097152)) > part2.bin
# cat part1.bin part2.bin > "$output"

# # Очистка
# rm part1.bin part2.bin

# ls -lh test.bin test_delete.bin  

# echo "Удалено 1 МБ на позиции $cut_pos. Результат в $output"

#!/bin/bash

# file="test.bin"
# output="test_insert.bin"
# insert_size=2097152  # 1 МБ = 1048576 байт

# # Получаем размер файла
# file_size=$(wc -c < "$file" | tr -d ' ')

# # Генерируем случайную позицию для вставки
# insert_pos=$(jot -r 1 0 $file_size)    # macOS

# # Создаем временный файл со случайными данными для вставки
# temp_insert="insert_temp.bin"
# dd if=/dev/urandom of="$temp_insert" bs=$insert_size count=1

# # Разделяем исходный файл и добавляем случайные данные
# dd if="$file" bs=1 count=$insert_pos > part1.bin
# dd if="$file" bs=1 skip=$insert_pos > part2.bin

# # Собираем новый файл
# cat part1.bin "$temp_insert" part2.bin > "$output"

# # Очистка
# rm -f part1.bin part2.bin "$temp_insert"

# ls -lh original.bin modified_insert.bin          

# echo "Добавлено 1 МБ случайных данных на позицию $insert_pos. Результат в $output"
# echo "Новый размер файла: $((file_size + insert_size)) байт"