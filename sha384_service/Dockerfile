# используем официальный образ GCC
FROM gcc:13

# устанавливаем OpenSSL
RUN apt-get update && apt-get install -y libssl-dev

# копируем код
COPY main.cpp /app/main.cpp

WORKDIR /app

# собираем программу
RUN g++ main.cpp -o sha384_app -lssl -lcrypto

# по умолчанию запускаем бинарник
CMD ["./sha384_app"]