# Диаграмма классов

В проекте реализованы следующие основные компоненты:

```
+----------------------+
|      SHAHasher       |
+----------------------+
| + sha384(input)      |
+----------------------+

+----------------------+
|      DESEncryptor    |
+----------------------+
| + des_encrypt_block(block, key) |
+----------------------+

+----------------------+
|  ChordMethodSolver   |
+----------------------+
| + chordMethod(func, x0, x1, epsilon, max_iter) |
+----------------------+

+----------------------+
|   SteganographyBMP   |
+----------------------+
| + encodeMessage(inputBmpPath, outputBmpPath, message) |
| + decodeMessage(filePath)                            |
+----------------------+
```
