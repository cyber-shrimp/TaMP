#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

// Проверка, что символ латиница A-Z или a-z или пробел
bool isLatinChar(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == ' ');
}

// Кодируем сообщение в BMP-файл, создавая новый BMP с внедрённым сообщением
bool encodeMessage(const string& inputBmpPath, const string& outputBmpPath, const string& message) {
    // Проверяем сообщение на латиницу
    for (char c : message) {
        if (!isLatinChar(c)) {
            cerr << "Wrong alphabet." << endl;
            return false;
        }
    }

    // Открываем исходный BMP-файл
    ifstream inputFile(inputBmpPath, ios::binary);
    if (!inputFile) {
        cerr << "I can't open your file." << endl;
        return false;
    }

    // Читаем весь файл в память
    vector<uint8_t> fileData((istreambuf_iterator<char>(inputFile)), {});
    inputFile.close();

    // Проверяем размер файла (минимум заголовок BMP)
    if (fileData.size() < 54) {
        cerr << "I don't think it is the right format." << endl;
        return false;
    }

    // Получаем смещение начала данных изображения из заголовка (байты 10-13)
    uint32_t dataOffset = *reinterpret_cast<uint32_t*>(&fileData[10]);
    if (dataOffset >= fileData.size()) {
        cerr << "Error." << endl;
        return false;
    }

    // Подготовка сообщения с завершающим нулём
    string msgWithTerminator = message + '\0';

    // Подсчитываем сколько битов нужно записать
    size_t bitsToWrite = msgWithTerminator.size() * 8;

    // Проверяем, что в данных изображения достаточно байтов для записи
    if (dataOffset + bitsToWrite > fileData.size()) {
        cerr << "Too long." << endl;
        return false;
    }

    // Внедрение сообщения в младшие биты пикселей
    size_t bitIndex = 0;
    for (char c : msgWithTerminator) {
        for (int bit = 0; bit < 8; bit++) {
            uint8_t bitVal = (c >> bit) & 1;
            size_t dataPos = dataOffset + bitIndex;
            fileData[dataPos] = (fileData[dataPos] & 0xFE) | bitVal;
            bitIndex++;
        }
    }

    // Записываем результат в новый файл
    ofstream outputFile(outputBmpPath, ios::binary);
    if (!outputFile) {
        cerr << "I can't make this file." << endl;
        return false;
    }

    outputFile.write(reinterpret_cast<char*>(fileData.data()), fileData.size());
    outputFile.close();

    cout << "Congratulations, your secrets are a little bit safer now here: " << outputBmpPath << endl;
    return true;
}

// Декодер сообщения из BMP (только латиница + пробел)
string decodeMessage(const string& filePath) {
    ifstream inputFile(filePath, ios::binary);
    if (!inputFile) {
        cerr << "Error: I can't open your image." << endl;
        return "";
    }

    // Читаем заголовок BMP (54 байта)
    vector<uint8_t> header(54);
    inputFile.read(reinterpret_cast<char*>(header.data()), header.size());
    if (inputFile.gcount() != 54) {
        cerr << "Nope, this is not BMP." << endl;
        return "";
    }

    // Определяем начало данных изображения
    uint32_t dataOffset = *reinterpret_cast<uint32_t*>(&header[10]);

    // Переходим к данным изображения
    inputFile.seekg(dataOffset, ios::beg);
    if (!inputFile) {
        cerr << "Error: I can't." << endl;
        return "";
    }

    // Читаем все данные пикселей
    vector<uint8_t> pixelData((istreambuf_iterator<char>(inputFile)), {});
    if (pixelData.empty()) {
        cerr << "Seems like this is just the picture." << endl;
        return "";
    }

    string decodedMessage;
    uint8_t currentByte = 0;
    int bitIndex = 0;

    for (uint8_t pixelByte : pixelData) {
        uint8_t lsb = pixelByte & 1;
        currentByte |= (lsb << bitIndex);
        bitIndex++;

        if (bitIndex == 8) {
            if (currentByte == '\0') {
                // Нулевой байт — конец сообщения
                break;
            }

            // Проверяем символ: латиница или пробел
            if ((currentByte >= 'A' && currentByte <= 'Z') ||
                (currentByte >= 'a' && currentByte <= 'z') ||
                (currentByte == ' ')) {
                decodedMessage += static_cast<char>(currentByte);
            }
            else {
                cerr << "Hmm, I see something I don't know." << endl;
                return "";
            }

            currentByte = 0;
            bitIndex = 0;
        }
    }

    return decodedMessage;
}

int main() {
    string filePath, inputPath, outputPath, message;
    int i;
    cout << "What is my purpose" << endl << "1. Encode" << endl << "2. Decode" << endl << "3. exit" << endl;
    cin >> i;
    switch (i) {
    case 1:
        
        cout << "What do you need to change: ";
        cin >> inputPath;

        cout << "Where do you need your image: ";
        cin >> outputPath;

        cout << "FEED ME YOUR SECRETS (only latin alphabet): ";
        cin.ignore(); // Сбрасываем символ новой строки после cin
        getline(cin, message);

        if (!encodeMessage(inputPath, outputPath, message)) {
            cerr << "Something went terribly wrong." << endl;
            return 1;
        }
        break;
    case 2:
        
        cout << "FEED ME YOUR SECRETS (BMP): ";
        cin >> filePath;

        cout << " " << filePath << endl;

        string message = decodeMessage(filePath);

        if (!message.empty()) {
            cout << "Congratulations, here is your message: " << message << endl;
        }
        else {
            cerr << "Something is wrong." << endl;
        }

    }
        
    

    return 0;
}
