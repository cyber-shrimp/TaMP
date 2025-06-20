# Используем образ с g++
FROM gcc:13

# Копируем исходный файл
COPY sha384.cpp /app/sha384.cpp

# Компилируем
RUN g++ -std=c++17 /app/sha384.cpp -o /app/sha384

# Запуск
WORKDIR /app
ENTRYPOINT ["./sha384"]
