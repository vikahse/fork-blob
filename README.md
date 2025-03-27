# fork-blob

### Решение было протестировано на файлах размера 1GB и 100MB, различие было только в 1 чанке, файлы были взяты [отсюда]([/guides/content/editing-an-existing-page](https://ash-speed.hetzner.com/)) 

#### Пример для запуска теста с бинарником размера 100MB, который лежит на гитхабе:

```
bash blob_test.sh
```
---

#### Либо прогон тестов вручную на более больших файлах

#### Создание второго файла, с изменением 1 байта:

```
cp test.bin test_modified.bin
printf '\x01' | dd of=test_modified.bin bs=1 seek=500000 conv=notrunc
```

#### Команды для запуска и проверки работы алгоритма:

```
gcc -o main main.c rollsum.c -lcrypto
./main test.bin > orig.manifest
./main test_modified.bin > modified.manifest
diff orig.manifest modified.manifest
```
---

Пример вывода разницы манифестов:

```
<   "131072:31cbafc3666cd7616bf272fc79ab0332a9a2b4624565f05ac4a4560f41842f67",
---
>   "131072:57558650e5b29302e40e181b38e8d38d1a174c31ccde03e3fc9b828360d72566",
```


